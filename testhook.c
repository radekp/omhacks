#include <stdio.h>
#include <omhacks/led.h>
#include <string.h>

typedef int hook_function(const char* name, const char* param);
extern void hooks_add_function(const char* name, hook_function func);

static struct om_led status_led;
static struct om_led status_led_saved;

static int test_hook(const char* name, const char* param)
{
	fprintf(stderr, "TH(%s,%s) ", name, param);
	return 0;
}

static int hook_status_led(const char* name, const char* param)
{
	if (strcmp(name, "00-statusled") == 0)
		if (strcmp(param, "suspend") == 0)
		{
			fprintf(stderr, "ON WITH THEIR LED!\n");
			// Save blue led state and turn it on
			om_led_get(&status_led_saved);
			status_led.brightness = 255;
			om_led_set(&status_led);
		}
		else
		{
			fprintf(stderr, "BACK WITH THEIR LED!\n");
			// Restore blue led state
			om_led_set(&status_led_saved);
		}
	else
		if (strcmp(param, "suspend") == 0)
		{
			fprintf(stderr, "OFF WITH THEIR LED!\n");
			// Turn off blue state before suspend
			status_led.brightness = 0;
			om_led_set(&status_led);
		}
		else
		{
			fprintf(stderr, "ON WITH THEIR LED!\n");
			// Turn on blue led after resume
			status_led.brightness = 255;
			om_led_set(&status_led);
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

	hooks_add_function("50-test1", test_hook);
	hooks_add_function("30-test2", test_hook);
}
