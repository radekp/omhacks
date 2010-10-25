#include <stdio.h>
#include <omhacks/screen.h>
#include <omhacks/led.h>
#include <omhacks/resumereason.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef int hook_function(int argc, const char* argv[]);
extern void hooks_add_function(const char* name, hook_function func);

static struct om_led status_led;
static struct om_led status_led_saved;
static int screen_brightness_saved = 255;
static int touchscreen_fd = -1;

static int hook_00_status_led(int argc, const char* argv[])
{
    if (argc < 2) return 254;

    if (strcmp(argv[1], "suspend") == 0)
    {
        // Save blue led state and turn it on
        if (om_led_get(&status_led_saved) < 0)
        {
            perror("reading blue led status");
            status_led.brightness = -1;
        } else {
            status_led.brightness = om_screen_brightness_get_max();
            om_led_set(&status_led);
        }
        return 0;
    }
    else if (strcmp(argv[1], "resume") == 0)
    {
        // Restore blue led state
        if (status_led.brightness != -1)
            om_led_set(&status_led_saved);
        return 0;
    }

    return 254;
}

static int hook_99_status_led(int argc, const char* argv[])
{
    if (argc < 2) return 254;

    if (strcmp(argv[1], "suspend") == 0)
    {
        // Turn off blue state before suspend
        if (status_led.brightness != -1)
        {
            status_led.brightness = 0;
            om_led_set(&status_led);
        }
        return 0;
    }
    else if (strcmp(argv[1], "resume") == 0)
    {
        // Turn on blue led after resume
        if (status_led.brightness != -1)
        {
            status_led.brightness = om_screen_brightness_get_max();
            om_led_set(&status_led);
        }
        return 0;
    }

    return 254;
}

static int hook_screen(int argc, const char* argv[])
{
    if (argc < 2) return 254;

    if (strcmp(argv[1], "suspend") == 0)
    {
        // Lock touchscreen
        touchscreen_fd = om_touchscreen_open();
        if (touchscreen_fd < 0)
            perror("opening touchscreen");
        else
            if (om_touchscreen_lock(touchscreen_fd) < 0)
                perror("locking touchscreen");

        // Save current backlight brightness and turn it off
        screen_brightness_saved = om_screen_brightness_swap(0);
        if (screen_brightness_saved < 0)
            screen_brightness_saved = om_screen_brightness_get_max();
        return 0;
    }
    else if (strcmp(argv[1], "resume") == 0)
    {
        // Restore saved backlight brightness
        om_screen_brightness_set(screen_brightness_saved);

        // Unlock touchscreen
        if (touchscreen_fd >= 0)
        {
            if (close(touchscreen_fd) < 0)
                perror("closing touchscreen");
            touchscreen_fd = -1;
        }
        return 0;
    }

    return 254;
}

static int hook_cancel_on_usb_disconnect(int argc, const char* argv[])
{
    if (argc < 2) return 254;

    if (strcmp(argv[1], "resume") == 0)
    {
        const char** resume_reason = om_resume_reason();
        if (resume_reason == NULL)
        {
            perror("getting resume reason");
            return 254;
        }
        for ( ; *resume_reason != NULL; ++resume_reason)
            if (strcmp(*resume_reason, "EINT09_PMU:usb_disconnect") == 0)
                return 250;
        return 0;
    }
    return 254;
}

void init()
{
    if (om_led_init(&status_led, "gta02-power:blue") == 0
     && om_led_init_copy(&status_led_saved, &status_led) == 0)
    {
        hooks_add_function("00-statusled", hook_00_status_led);
        hooks_add_function("99-statusled", hook_99_status_led);
    }

    hooks_add_function("74-screen", hook_screen);
    hooks_add_function("98-cancel-on-usb-disconnect", hook_cancel_on_usb_disconnect);
}
