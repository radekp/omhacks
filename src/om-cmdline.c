/*
 * omhacks - Various useful utility functions for the FreeRunner
 *
 * Copyright (C) 2009  Enrico Zini <enrico@enricozini.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <omhacks/all.h>
#include "om-cmdline.h"
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

const char* argv0;
struct opt_t opts;

int om_flags_sysfs = 0;
int om_flags_backlight = 0;
int om_flags_touchscreen = 0;
int om_flags_bt = 0;
int om_flags_gsm = 0;
int om_flags_gps = 0;
int om_flags_wifi = 0;
int om_flags_resume_reason = 0;
int om_flags_led = 0;

void usage_sysfs(FILE* out)
{
	fprintf(out, "Usage: %s sysfs name [name...]\n", argv0);
}

int do_sysfs(int argc, char *const *argv)
{
	int i;
	if (argc == 1)
	{
		usage_sysfs(stderr);
		return 1;
	}
	for (i = 1; i < argc; ++i)
	{
		const char* res = om_sysfs_path(argv[i]);
		if (res == NULL)
		{
			fprintf(stderr, "%s not found\n", argv[i]);
			return 1;
		}
		puts(res);
	}
	return 0;
}

void usage_backlight(FILE* out)
{
	fprintf(out, "Usage: %s backlight\n", argv0);
	fprintf(out, "Usage: %s backlight <brightness>\n", argv0);
}

int do_backlight(int argc, char *const *argv)
{
	if (argc == 1)
	{
		int val = om_screen_brightness_get();
		if (val < 0)
		{
			perror("getting brightness");
			return 1;
		}
		printf("%d\n", val);
	}
	else
	{
		if (opts.swap)
		{
			int old_val = om_screen_brightness_swap(atoi(argv[1]));
			if (old_val < 0)
			{
				perror("getting/setting brightness");
				return 1;
			}
			printf("%d\n", old_val);
		} else {
			int res = om_screen_brightness_set(atoi(argv[1]));
			if (res < 0)
			{
				perror("setting brightness");
				return 1;
			}
		}
	}
	return 0;
}

void usage_touchscreen(FILE* out)
{
	fprintf(out, "Usage: %s touchscreen lock\n", argv0);
}

int do_touchscreen(int argc, char *const *argv)
{
	if (argc == 1 || strcmp(argv[1], "lock") != 0)
	{
		usage_touchscreen(stderr);
		return 1;
	}
	int ts = om_touchscreen_open();
	if (ts < 0)
	{
		perror("opening touchscreen");
		return 1;
	}
	if (om_touchscreen_lock(ts) < 0)
	{
		perror("locking touchscreen");
		return 1;
	}
	pause();
	if (close(ts) < 0)
	{
		perror("closing touchscreen");
		return 1;
	}
	return 0;
}

void usage_bt(FILE* out)
{
	fprintf(out, "Usage: %s bt [--swap] power [1/0]\n", argv0);
}

int do_bt(int argc, char *const *argv)
{
	if (argc == 1)
	{
		usage_bt(stderr);
		return 1;
	}
	if (strcmp(argv[1], "power") == 0)
	{
		if (argc == 2)
		{
			int res = om_bt_power_get();
			if (res < 0)
			{
				perror("reading BlueTooth power");
				return 1;
			}
			printf("%d\n", res);
		} else {
			if (opts.swap)
			{
				int res = om_bt_power_swap(atoi(argv[2]));
				if (res < 0)
				{
					perror("reading/setting BlueTooth power");
					return 1;
				}
				printf("%d\n", res);
			} else {
				int res = om_bt_power_set(atoi(argv[2]));
				if (res < 0)
				{
					perror("setting BlueTooth power");
					return 1;
				}
			}
		}
	} else {
		usage_bt(stderr);
		return 1;
	}
	return 0;
}

void usage_gsm(FILE* out)
{
	fprintf(out, "Usage: %s gsm [--swap] power [1/0]\n", argv0);
}

int do_gsm(int argc, char *const *argv)
{
	if (argc == 1)
	{
		usage_gsm(stderr);
		return 1;
	}
	if (strcmp(argv[1], "power") == 0)
	{
		if (argc == 2)
		{
			int res = om_gsm_power_get();
			if (res < 0)
			{
				perror("reading GSM power");
				return 1;
			}
			printf("%d\n", res);
		} else {
			if (opts.swap)
			{
				int res = om_gsm_power_swap(atoi(argv[2]));
				if (res < 0)
				{
					perror("reading/setting GSM power");
					return 1;
				}
				printf("%d\n", res);
			} else {
				int res = om_gsm_power_set(atoi(argv[2]));
				if (res < 0)
				{
					perror("setting GSM power");
					return 1;
				}
			}
		}
	} else {
		usage_gsm(stderr);
		return 1;
	}
	return 0;
}

void usage_gps(FILE* out)
{
	fprintf(out, "Usage: %s gps [--swap] power [1/0]\n", argv0);
	fprintf(out, "Usage: %s gps [--swap] keep-on-in-suspend [1/0]\n", argv0);
}

int do_gps(int argc, char *const *argv)
{
	if (argc == 1)
	{
		usage_gps(stderr);
		return 1;
	}
	if (strcmp(argv[1], "power") == 0)
	{
		if (argc == 2)
		{
			int res = om_gps_power_get();
			if (res < 0)
			{
				perror("reading GPS power");
				return 1;
			}
			printf("%d\n", res);
		} else {
			if (opts.swap)
			{
				int res = om_gps_power_swap(atoi(argv[2]));
				if (res < 0)
				{
					perror("reading/setting GPS power");
					return 1;
				}
				printf("%d\n", res);
			} else {
				int res = om_gps_power_set(atoi(argv[2]));
				if (res < 0)
				{
					perror("setting GPS power");
					return 1;
				}
			}
		}
	} else if (strcmp(argv[1], "keep-on-in-suspend") == 0) {
		if (argc == 2)
		{
			int res = om_gps_keep_on_in_suspend_get();
			if (res < 0)
			{
				perror("reading GPS keep-on-in-suspend");
				return 1;
			}
			printf("%d\n", res);
		} else {
			if (opts.swap)
			{
				int res = om_gps_keep_on_in_suspend_swap(atoi(argv[2]));
				if (res < 0)
				{
					perror("reading/setting GPS keep-on-in-suspend");
					return 1;
				}
				printf("%d\n", res);
			} else {
				int res = om_gps_keep_on_in_suspend_set(atoi(argv[2]));
				if (res < 0)
				{
					perror("setting GPS keep-on-in-suspend");
					return 1;
				}
			}
		}
	} else {
		usage_gps(stderr);
		return 1;
	}
	return 0;
}

void usage_resume_reason(FILE* out)
{
	fprintf(out, "Usage: %s resume-reason\n", argv0);
}

void usage_wifi(FILE* out)
{
	fprintf(out, "Usage: %s wifi [--swap] power [1/0]\n", argv0);
}

int do_wifi(int argc, char *const *argv)
{
	if (argc == 1)
	{
		usage_wifi(stderr);
		return 1;
	}
	if (strcmp(argv[1], "power") == 0)
	{
		if (argc == 2)
		{
			int res = om_wifi_power_get();
			if (res < 0)
			{
				perror("reading WiFi power");
				return 1;
			}
			printf("%d\n", res);
		} else {
			if (opts.swap)
			{
				int res = om_wifi_power_swap(atoi(argv[2]));
				if (res < 0)
				{
					perror("reading/setting WiFi power");
					return 1;
				}
				printf("%d\n", res);
			} else {
				int res = om_wifi_power_set(atoi(argv[2]));
				if (res < 0)
				{
					perror("setting WiFi power");
					return 1;
				}
			}
		}
	} else {
		usage_wifi(stderr);
		return 1;
	}
	return 0;
}

void usage_battery(FILE* out)
{
	fprintf(out, "Usage: %s battery temperature\n", argv0);
}

int do_battery(int argc, char *const *argv)
{
	if (argc == 1)
	{
		usage_battery(stderr);
		return 1;
	}
	if (strcmp(argv[1], "temperature") == 0)
	{
		if (argc == 2)
		{
			float temperature;
			int res = om_battery_temperature_get(&temperature);
			if (res < 0)
			{
				perror("reading battery temperature");
				return 1;
			}
			printf("%.2f\n", temperature);
		} else {
			usage_battery(stderr);
			return 1;
		}
	}
	return 0;
}

void usage_power(FILE* out)
{
	fprintf(out, "Usage: %s power\n", argv0);
	fprintf(out, "Usage: %s power all-off\n", argv0);
}

int do_power(int argc, char *const *argv)
{
	if (argc == 1)
	{
		int val;
		if ((val = om_bt_power_get()) < 0) return val;
		printf("bt power %d\n", val);
		if ((val = om_gsm_power_get()) < 0) return val;
		printf("gsm power %d\n", val);
		if ((val = om_gps_power_get()) < 0) return val;
		printf("gps power %d\n", val);
		if ((val = om_gps_keep_on_in_suspend_get()) < 0) return val;
		printf("gps keep-on-in-suspend %d\n", val);
		if ((val = om_wifi_power_get()) < 0) return val;
		printf("wifi power %d\n", val);
	}
	if (strcmp(argv[1], "all-off") == 0)
	{
		if (om_bt_power_set(0) < 0) return -1;
		if (om_gsm_power_set(0) < 0) return -1;
		if (om_gps_power_set(0) < 0) return -1;
		if (om_wifi_power_set(0) < 0) return -1;
	}
	return 0;
}

int do_resume_reason(int argc, char *const *argv)
{
	const char* res = om_resume_reason();
	if (res == NULL)
	{
		perror("getting resume reason");
		return 1;
	}
	puts(res);
	return 0;
}

void usage_led(FILE* out, const char* ledname)
{
	const char* ledcmd;
	if (om_flags_led & OM_FLAGS_LED_STANDALONE)
		ledcmd = "";
	else
		ledcmd = " led";

	if (ledname == NULL) ledname = "<name>";
	fprintf(out, "Usage: %s%s %s\n", argv0, ledcmd, ledname);
	fprintf(out, "Usage: %s%s %s <brightness>\n", argv0, ledcmd, ledname);
	fprintf(out, "Usage: %s%s %s <brightness> timer <ontime> <offtime>\n", argv0, ledcmd, ledname);
}

static int led_read_extra_args(struct om_led* led, int argc, char *const *argv)
{
	if (argc == 0)
	{
		strcpy(led->trigger, "none");
		led->delay_on = led->delay_off = 0;
	} else if (strcmp(argv[0], "timer") == 0 && argc == 3) {
		strcpy(led->trigger, "timer");
		led->delay_on = atoi(argv[1]);
		led->delay_off = atoi(argv[2]);
	} else if (strcmp(argv[0], "timer") != 0) {
		strncpy(led->trigger, argv[0], 254);
		led->trigger[254] = 0;
		led->delay_on = led->delay_off = 0;
	} else {
		usage_led(stderr, led->name);
		return 1;
	}
	return 0;
}

static void print_led(const struct om_led* led)
{
	if (strcmp(led->trigger, "none") == 0)
	{
		printf("%d\n", led->brightness);
	} else if (strcmp(led->trigger, "timer") == 0) {
		printf("%d timer %d %d\n", led->brightness, led->delay_on, led->delay_off);
	} else {
		printf("%d %s\n", led->brightness, led->trigger);
	}
}

int do_led(int argc, char *const *argv)
{
	struct om_led led;
	if (argc == 1)
	{
		// TODO: list all leds and their status instead
		DIR* dir = opendir("/sys/class/leds/");
		struct dirent* d;
		if (dir == NULL)
		{
			perror("opening /sys/class/leds");
			return 1;
		}
		while ((d = readdir(dir)) != NULL)
		{
			if (d->d_name[0] == '.') continue;
			printf("%s ", d->d_name);
			if (om_led_init(&led, d->d_name) != 0)
			{
				perror("accessing led");
				return 1;
			}
			if (om_led_get(&led) != 0)
			{
				perror("getting led status");
				return 1;
			}
			print_led(&led);
		}
		closedir(dir);
		return 0;
	}

	if (om_led_init(&led, argv[1]) != 0)
	{
		perror("validating led name");
		return 1;
	}

	if (argc == 2)
	{
		// Get status
		if (om_led_get(&led) != 0)
		{
			perror("getting led status");
			return 1;
		}
		print_led(&led);
	} else {
		if (opts.swap)
		{
			if (om_led_get(&led) != 0)
			{
				perror("getting led status");
				return 1;
			}
			print_led(&led);
		}

		led.brightness = atoi(argv[2]);

		if (led_read_extra_args(&led, argc-3, argv+3) != 0)
			return 1;

		if (om_led_set(&led) != 0)
		{
			perror("setting led status");
			return 1;
		}
	}
	return 0;
}

void usage_options(FILE* out)
{
	fprintf(out, "Options:\n");
	fprintf(out, "  --help: print this help message\n");
	fprintf(out, "  --swap: set new value and print old value\n");
}

int parse_options(int argc, char *const *argv)
{
	argv0 = argv[0];

	memset(&opts, 0, sizeof(struct opt_t));
        int option_index = 0;
        
        static struct option long_options[] = {
            {"help", 0, &opts.help, 1},
	    /*
            {"version", 0, 0, 0},
            {"brightness", 1, 0, 0},
            {"on", 0, 0, 0},
            {"off", 0, 0, 0},
            {"on-time", 1, 0, 0},
            {"off-time", 1, 0, 0},
            {"blink", 0, 0, 0},
            {"no-blink", 0, 0, 0},
            {"verbose", 0, 0, 0},
	    */
            {"swap", 0, &opts.swap, 1},
            {0, 0, 0, 0}
        };
        
	while (1)
	{
		int ret = getopt_long(argc, argv, "", long_options, &option_index);
		switch (ret)
		{
			case -1: return 0;
			case 0: break;
			default: return -1;
		}
	}
}


