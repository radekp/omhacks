#include <stdio.h>
#include <omhacks/screen.h>
#include <omhacks/led.h>
#include <omhacks/resumereason.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef int hook_function(const char* name, const char* param);
extern void hooks_add_function(const char* name, hook_function func);

static struct om_led status_led;
static struct om_led status_led_saved;
static int screen_brightness_saved = 255;
static int touchscreen_fd = -1;

static int hook_status_led(const char* name, const char* param)
{
	if (strcmp(name, "00-statusled") == 0)
	{
		if (strcmp(param, "suspend") == 0)
		{
			// Save blue led state and turn it on
			om_led_get(&status_led_saved);
			status_led.brightness = 255;
			om_led_set(&status_led);
		}
		else if (strcmp(param, "resume") == 0)
		{
			// Restore blue led state
			om_led_set(&status_led_saved);
		}
	}
	else if (strcmp(name, "99-statusled") == 0)
	{
		if (strcmp(param, "suspend") == 0)
		{
			// Turn off blue state before suspend
			status_led.brightness = 0;
			om_led_set(&status_led);
		}
		else if (strcmp(param, "resume") == 0)
		{
			// Turn on blue led after resume
			status_led.brightness = 255;
			om_led_set(&status_led);
		}
	}
	return 0;
}

static int hook_screen(const char* name, const char* param)
{
	if (strcmp(param, "suspend") == 0)
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
			screen_brightness_saved = 255;
	}
	else if (strcmp(param, "resume") == 0)
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
	}
	return 0;
}

static int hook_cancel_on_usb_disconnect(const char* name, const char* param)
{
	if (strcmp(param, "resume") == 0)
	{
		const char** resume_reason = om_resume_reason();
		for ( ; *resume_reason != NULL; ++resume_reason)
			if (strcmp(*resume_reason, "EINT09_PMU:usb_disconnect") == 0)
				return 250;
	}
	return 0;
}
void init()
{
	if (om_led_init(&status_led, "gta02-power:blue") == 0
	 && om_led_init_copy(&status_led_saved, &status_led) == 0)
	{
		hooks_add_function("00-statusled", hook_status_led);
		hooks_add_function("99-statusled", hook_status_led);
	}

	hooks_add_function("74-screen", hook_screen);
	hooks_add_function("98-cancel-on-usb-disconnect", hook_cancel_on_usb_disconnect);
}
