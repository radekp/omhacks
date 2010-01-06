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
int om_flags_uevent = 0;

static const char* om_usage_lead = NULL;

void usage_lead_reset()
{
	om_usage_lead = "Usage: ";
}

const char* usage_lead()
{
	const char* res = om_usage_lead;
	om_usage_lead = "   or: ";
	return res;
}

void usage_sysfs(FILE* out)
{
	fprintf(out, "%s %s sysfs name [name...]\n", usage_lead(), argv0);
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
	fprintf(out, "%s %s backlight\n", usage_lead(), argv0);
	fprintf(out, "%s %s backlight get-max\n", usage_lead(), argv0);
	fprintf(out, "%s %s backlight <brightness>\n", usage_lead(), argv0);
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
		if (strcmp(argv[1], "get-max") == 0)
		{
			int val = om_screen_brightness_get_max();
			if (val < 0)
			{
				perror("getting max brightness value");
				return 1;
			}
			printf("%d\n", val);
		} else if (opts.swap) {
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
	fprintf(out, "%s %s touchscreen lock\n", usage_lead(), argv0);
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
	fprintf(out, "%s %s bt [--swap] power [1/0]\n", usage_lead(), argv0);
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
	fprintf(out, "%s %s gsm [--swap] power [1/0]\n", usage_lead(), argv0);
	fprintf(out, "%s %s gsm flowcontrol [1/0]\n", usage_lead(), argv0);
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
	} else if (strcmp(argv[1], "flowcontrol") == 0) {
		if (argc == 2)
		{
			int res = om_gsm_flowcontrol_get();
			if (res < 0)
			{
				perror("reading GSM flowcontrol");
				return 1;
			}
			printf("%d\n", res);
		} else {
			int res = om_gsm_flowcontrol_set(atoi(argv[2]));
			if (res < 0)
			{
				perror("settings GSM flowcontrol");
				return 1;
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
	fprintf(out, "%s %s gps [--swap] power [1/0]\n", usage_lead(), argv0);
	fprintf(out, "%s %s gps [--swap] keep-on-in-suspend [1/0]\n", usage_lead(), argv0);
	fprintf(out, "%s %s gps send-ubx <class> <type> [payload_byte0] [payload_byte1] ...\n", usage_lead(), argv0);
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
        } else if (strcmp(argv[1], "send-ubx") == 0) {
            if (argc < 4)
            {
                usage_gps(stderr);
                return 1;
            } else {
                char *payload;
                int klass, type, payloadlen, fd, res, i;

                fd = om_gps_open();
                if (fd < 0) {
                    perror("opening GPS device");
                    return 1;
                }
                klass = strtol(argv[2], NULL, 16);
                type = strtol(argv[3], NULL, 16);
                payloadlen = argc - 4;

                payload = malloc(payloadlen);
                if (payload == NULL) {
                    perror("allocating memory for UBX packet");
                    return 1;
                }
                for (i = 0; i < payloadlen; i++) {
                    payload[i] = strtol(argv[4 + i], NULL, 16);
                }

                res = om_gps_ubx_send(fd, klass, type, payload, payloadlen);
                if (res < 0) {
                    perror("sending UBX packet");
                }
                om_gps_close(fd);
                free(payload);
                return (res < 0) ? 1 : 0;
            }
	} else {
		usage_gps(stderr);
		return 1;
	}
	return 0;
}

void usage_wifi(FILE* out)
{
	fprintf(out, "%s %s wifi [--swap] power [1/0]\n", usage_lead(), argv0);
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
	fprintf(out, "%s %s battery temperature\n", usage_lead(), argv0);
	fprintf(out, "%s %s battery energy\n", usage_lead(), argv0);
	fprintf(out, "%s %s battery consumption\n", usage_lead(), argv0);
	fprintf(out, "%s %s battery charger-limit [0-500]\n", usage_lead(), argv0);
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
		}
		else
		{
			usage_battery(stderr);
			return 1;
		}
	}
	else if (strcmp(argv[1], "energy") == 0)
	{
		if (argc == 2)
		{
			int res = om_battery_energy_get();
			if (res < 0)
			{
				perror("reading battery energy");
				return 1;
			}
			printf("%d\n", res);
		}
		else
		{
			usage_battery(stderr);
			return 1;
		}
	}
	else if (strcmp(argv[1], "consumption") == 0)
	{
		if (argc == 2)
		{
			int consumption;
			int res = om_battery_consumption_get(&consumption);
			if (res < 0)
			{
				perror("reading battery consumption");
				return 1;
			}
			printf("%d\n", consumption);
		}
		else
		{
			usage_battery(stderr);
			return 1;
		}
        }
        else if (strcmp(argv[1], "charger-limit") == 0)
        {
		if (argc == 2)
		{
			int res = om_battery_charger_limit_get();
			if (res < 0)
			{
				perror("reading battery charger limit");
				return 1;
			}
			printf("%d\n", res);
			return 0;
		} else if (argc == 3) {
			int res = om_battery_charger_limit_set(atoi(argv[2]));
			if (res < 0)
			{
				perror("setting battery charger limit");
				return 1;
			}
			return 0;
		}
	}
	else
	{
		usage_battery(stderr);
		return 1;
	}
	return 0;
}

void usage_power(FILE* out)
{
	fprintf(out, "%s %s power\n", usage_lead(), argv0);
	fprintf(out, "%s %s power all-off\n", usage_lead(), argv0);
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
	else if (strcmp(argv[1], "all-off") == 0)
	{
		if (om_bt_power_set(0) < 0) return 1;
		if (om_gsm_power_set(0) < 0) return 1;
		if (om_gps_power_set(0) < 0) return 1;
		if (om_wifi_power_set(0) < 0) return 1;
	} else {
		usage_power(stderr);
		return 1;
	}
	return 0;
}

void usage_resume_reason(FILE* out)
{
	fprintf(out, "%s %s resume-reason\n", usage_lead(), argv0);
	fprintf(out, "%s %s resume-reason contains <val>\n", usage_lead(), argv0);
}

int do_resume_reason(int argc, char *const *argv)
{
	const char** res = om_resume_reason();
	if (res == NULL)
	{
		perror("getting resume reason");
		return 1;
	}
	if (argc == 1)
	{
		for ( ; *res != NULL; ++res)
			puts(*res);
	} else if (strcmp(argv[1], "contains") == 0 && argc == 3) {
		int found = 0;
		for ( ; *res != NULL && !found; ++res)
			if (strcmp(*res, argv[2]) == 0)
				found = 1;
		if (!found) return 1;
	} else {
		usage_power(stderr);
		return 1;
	}
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
	fprintf(out, "%s %s %s %s\n", usage_lead(), argv0, ledcmd, ledname);
	fprintf(out, "%s %s %s %s <brightness>\n", usage_lead(), argv0, ledcmd, ledname);
	fprintf(out, "%s %s %s %s <brightness> timer <ontime> <offtime>\n", usage_lead(), argv0, ledcmd, ledname);
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

void usage_uevent(FILE* out)
{
	fprintf(out, "%s %s uevent dump\n", usage_lead(), argv0);
}

int do_uevent(int argc, char *const *argv)
{
	if (argc == 1)
	{
		usage_uevent(stderr);
		return 1;
	} else if (strcmp(argv[1], "dump") == 0 && argc == 2) {
		int sock = om_uevent_open();
		struct om_uevent ou;
		if (sock < 0)
		{
			perror("opening uevent socket");
			return 1;
		}
		while (1)
		{
			if (om_uevent_read(sock, &ou) < 0)
			{
				perror("reading from uevent socket");
				return 1;
			}

			if (om_uevent_parse(&ou) == 0)
			{
				int i;
				printf("OU_ACTION=%s\n", ou.action);
				printf("OU_DEVPATH=%s\n", ou.devpath);
				for (i = 0; ou.envp[i] != NULL; i++)
					printf("%s\n", ou.envp[i]);
				putchar('\n');
			} else {
				fprintf(stderr, "Skipping unparsed event %s\n", ou.buffer);
			}
		}
	} else {
		usage_uevent(stderr);
		return 1;
	}
	return 0;
}

void usage_usb(FILE* out)
{
    fprintf(out, "%s %s usb mode [device|host]\n", usage_lead(), argv0);
    fprintf(out, "%s %s usb charger-mode [charge-battery|power-usb]\n", usage_lead(), argv0);
    fprintf(out, "%s %s usb charger-limit [0|100|500]\n", usage_lead(), argv0);
}

int do_usb(int argc, char *const *argv)
{
    if (argc == 1)
    {
        usage_usb(stderr);
    } else if (strcmp(argv[1], "mode") == 0) {
        if (argc == 2)
        {
            int res = om_usb_mode_get();
            if (res < 0 || res > 1)
            {
                perror("reading usb mode");
                return 1;
            }
            printf("%s\n", res == 0 ? "device" : "host");
        } else if (argc == 3) {
            int res;
            if (strcmp(argv[2], "device") == 0)
            {
                res = om_usb_mode_set(0);
            } else if (strcmp(argv[2], "host") == 0) {
                res = om_usb_mode_set(1);
            } else {
                usage_usb(stderr);
                return 1;
            }
            if (res < 0)
            {
                perror("setting usb mode");
                return 1;
            }
            return 0;
        } else {
            usage_usb(stderr);
            return 1;
        }
    } else if (strcmp(argv[1], "charger-mode") == 0) {
        if (argc == 2)
        {
            int res = om_usb_charger_mode_get();
            if (res < 0)
            {
                perror("reading usb charger mode");
                return 1;
            }
            printf("%s\n", res == 0 ? "charge-battery" : "power-usb");
        } else if (argc == 3) {
            int res;
            if (strcmp(argv[2], "charge-battery") == 0) {
                res = om_usb_charger_mode_set(0);
            } else if (strcmp(argv[2], "power-usb") == 0) {
                res = om_usb_charger_mode_set(1);
            } else {
                usage_usb(stderr);
                return 1;
            }
            if (res < 0)
            {
                perror("setting usb charger mode");
                return 1;
            }
            return 0;
        }
    } else if (strcmp(argv[1], "charger-limit") == 0)
    {
        if (argc == 2)
        {
            int res = om_usb_charger_limit_get();
            if (res < 0)
            {
                perror("reading usb charger limit");
                return 1;
            }
            printf("%d\n", res);
            return 0;
        } else if (argc == 3) {
            int res = om_usb_charger_limit_set(atoi(argv[2]));
            if (res < 0)
            {
                perror("setting usb charger limit");
                return 1;
            }
            return 0;
        }
    } else {
        usage_usb(stderr);
        return 1;
    }
    return 0;
}

int do_version(int argc, char *const *argv)
{
	printf("%s version %s\n", argv0, VERSION);
	return 0;
}

void usage_options(FILE* out)
{
	fprintf(out, "Options:\n");
	fprintf(out, "  --help:    print this help message\n");
	fprintf(out, "  --version: print version and exit\n");
	fprintf(out, "  --swap:    set new value and print old value\n");
}

int parse_options(int argc, char *const *argv)
{
	argv0 = argv[0];

	memset(&opts, 0, sizeof(struct opt_t));
        int option_index = 0;
        
        static struct option long_options[] = {
            {"help", 0, &opts.help, 1},
            {"version", 0, &opts.version, 1},
	    /*
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


