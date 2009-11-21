#include <stdio.h>
#include <omhacks/sysfs.h>
#include <omhacks/led.h>
#include <string.h>
#include <stdlib.h>

typedef int hook_function(const char* name, const char* param);
extern void hooks_add_function(const char* name, hook_function func);

static struct om_led status_led;
static struct om_led status_led_saved;
static int screen_brightness_saved;

static int hook_status_led(const char* name, const char* param)
{
	if (strcmp(name, "00-statusled") == 0)
		if (strcmp(param, "suspend") == 0)
		{
			// Save blue led state and turn it on
			om_led_get(&status_led_saved);
			status_led.brightness = 255;
			om_led_set(&status_led);
		}
		else
		{
			// Restore blue led state
			om_led_set(&status_led_saved);
		}
	else
		if (strcmp(param, "suspend") == 0)
		{
			// Turn off blue state before suspend
			status_led.brightness = 0;
			om_led_set(&status_led);
		}
		else
		{
			// Turn on blue led after resume
			status_led.brightness = 255;
			om_led_set(&status_led);
		}
	return 0;
}

static int hook_screen(const char* name, const char* param)
{
	if (strcmp(param, "suspend") == 0)
	{
		// Save current backlight brightness and turn it off
		const char* val = om_sysfs_swap("brightness", "0");
		if (val == NULL)
			screen_brightness_saved = 255;
		else
			screen_brightness_saved = atoi(val);
	}
	else
	{
		// Restore saved backlight brightness
		char val[20];
		snprintf(val, 20, "%d", screen_brightness_saved);
		om_sysfs_set("brightness", val);
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
	hooks_add_function("74-screen", hook_screen);
}
