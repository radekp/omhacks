/*
 * pm-action - lightweight, simple-minded pm-utils replacement
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <errno.h>

/* Timing subsystem */
#define MAX_TIMING 5

static struct timeval timing_pre[MAX_TIMING];
static int timing_cur = 0;

void timing_start()
{
	if (timing_cur < MAX_TIMING)
		gettimeofday(&timing_pre[timing_cur], NULL);
	timing_cur++;
}

long int timing_end()
{
	struct timeval post;
	--timing_cur;
	if (timing_cur >= MAX_TIMING) return 0;
	gettimeofday(&post, NULL);
	return (post.tv_sec - timing_pre[timing_cur].tv_sec) * 1000000
             + (post.tv_usec - timing_pre[timing_cur].tv_usec);
}

/* Hooks subsystem */

#define MAX_HOOKS 1024

struct hook {
	char* name;
	const char* dirname;
	int priority;
	int active;
};

static struct hook hooks[MAX_HOOKS];
static int hooks_size = 0;
static FILE* log = NULL;

static int read_hooks(const char* dir, int priority)
{
	DIR* fdir = opendir(dir);
	char fullname[PATH_MAX];
	struct dirent* d;
	struct stat st;
	int res = 0;

	if (fdir == NULL)
		return res;

	while ((d = readdir(fdir)) != NULL)
	{
		if (d->d_name[0] == '.') continue;
		snprintf(fullname, PATH_MAX, "%s/%s", dir, d->d_name);
		if (stat(fullname, &st) != 0) continue;
		// Skip directories
		if (S_ISDIR(st.st_mode)) continue;

		hooks[hooks_size].name = strdup(d->d_name);
		if (hooks[hooks_size].name == NULL)
		{
			res = -1;
			break;
		}
		hooks[hooks_size].dirname = dir;
		hooks[hooks_size].priority = priority;
		// If a file is executable, do not run it, but take note as it
		// can override an executable file elsewhere
		hooks[hooks_size].active = ((st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) != 0);
		++hooks_size;
		if (hooks_size == MAX_HOOKS)
			break;
	}

	if (fdir != NULL) closedir(fdir);
	return res;
}

static int hook_compare(const void *a, const void *b)
{
	const struct hook* ha = (const struct hook*) a;
	const struct hook* hb = (const struct hook*) b;
	int res = strcmp(ha->name, hb->name);
	if (res == 0)
		return hb->priority - ha->priority;
	else
		return res;
}

static void sort_hooks()
{
	qsort(hooks, hooks_size, sizeof(struct hook), hook_compare);
}

static void filter_hooks()
{
	int last = 0;
	int i;
	for (i = 1; i < hooks_size; ++i)
	{
 		if (!hooks[i].active)
		{
			while (i < hooks_size && strcmp(hooks[i].name, hooks[i+1].name) == 0)
				++i;
			continue;
		}
		if (strcmp(hooks[last].name, hooks[i].name) == 0)
			continue;
		if (last + 1 != i)
			memcpy(&hooks[last+1], &hooks[i], sizeof(struct hook));
		++last;
	}
	hooks_size = last;
}

static void print_hooks(FILE* out)
{
	int i;
	for (i = 0; i < hooks_size; ++i)
	{
		fprintf(out, "%3i: %s/%s\n", i+1, hooks[i].dirname, hooks[i].name);
	}
}

static int run_hook(int hook, const char* parm)
{
	char cmd[PATH_MAX + 30];
	int res;
	if (!hooks[hook].active) return 0;

	snprintf(cmd, PATH_MAX + 30, "%s/%s %s", hooks[hook].dirname, hooks[hook].name, parm);
	fprintf(stderr, "Running %s... ", cmd);
	timing_start();
	res = system(cmd);
	long int elapsed = timing_end();
	if (!WIFEXITED(res))
		return res;
	res = WEXITSTATUS(res);
	fprintf(stderr, "result: %d, elapsed: %8ldusec\n", res, elapsed);
	switch (res)
	{
		case 254: // not applicable
		case 253: // not executable
		case 252: // disabled
			return 0;
		case 250: // special: reverse
			return -1;
		default:
			// Inhibit further execution
			return res;
	}
}

static int do_suspend()
{
	int fd = open("/sys/power/state", O_WRONLY);
	int res = 0;
	if (fd < 0) return fd;

	if (write(fd, "mem\n", 4) != 4)
	{
		res = -1;
		goto cleanup;
	}

cleanup:
	close(fd);
	return res;
}

static int endswith(const char* str, int strsize, const char* test)
{
	size_t testsize = strlen(test);
	if (strsize < testsize) return 0;
	return strcmp(str + strsize - testsize, test) == 0;
}

static const char* getname(const char* argv0)
{
	size_t a0len = strlen(argv0);
	if (endswith(argv0, a0len, "pm-suspend"))
		return "pm-suspend";
	if (endswith(argv0, a0len, "pm-suspend-light"))
		return "pm-suspend";
	if (endswith(argv0, a0len, "pm-suspend-lite"))
		return "pm-suspend";
	return NULL;
}

static int makedirs(const char* stashname)
{
	char buf[PATH_MAX];
	int res;
	res = mkdir("/var/run/pm-utils", 0755);
	if (res < 0 && errno != EEXIST) return res;
	snprintf(buf, PATH_MAX, "/var/run/pm-utils/%s", stashname);
	res = mkdir(buf, 0755);
	if (res < 0 && errno != EEXIST) return res;
	snprintf(buf, PATH_MAX, "/var/run/pm-utils/%s/storage", stashname);
	res = mkdir(buf, 0755);
	if (res < 0 && errno != EEXIST) return res;
	return 0;
}

int main(int argc, const char* argv[])
{
	const char* forwards = "suspend"; // hibernate
	const char* backwards = "resume"; // thaw
	const char* logfile = "/var/log/pm-suspend.log";
	const char* name = getname(argv[0]);
	int logfd;

	if (name == NULL)
	{
		fprintf(stderr, "You called me with an invalid name\n");
		return 1;
	}

	if (getuid() != 0)
	{
		fprintf(stderr, "This utility may only be run by the root user.\n");
		return 1;
	}

	// Open logfile
	logfd = open(logfile, O_WRONLY | O_APPEND | O_CREAT | O_TRUNC);
	if (logfd < 0)
	{
		fprintf(stderr, "Cannot write to %s: %s\n", logfile, strerror(errno));
		return 1;
	}
	log = fdopen(logfd, "a");
	if (log == NULL)
	{
		fprintf(stderr, "Cannot fdopen %s: %s\n", logfile, strerror(errno));
		return 1;
	}
	if (dup2(logfd, 1) < 0)
	{
		fprintf(stderr, "Sending stdout to %s: %s\n", logfile, strerror(errno));
		return 1;
	}
	if (dup2(1, 2) < 0)
	{
		fprintf(stderr, "Sending stderr to %s: %s\n", logfile, strerror(errno));
		return 1;
	}

	// Setup the environment
	if (makedirs(name) < 0)
	{
		perror("Creating state dir: ");
		return 1;
	}
	setenv("PM_FUNCTIONS", "/usr/lib/pm-utils/pm-functions", 1);
	setenv("PM_LOGFILE", logfile, 1);
	setenv("PM_UTILS_RUNDIR", "/var/run/pm-utils", 1);
	setenv("STASHNAME", name, 1);
	setenv("NA", "254", 1);
	setenv("NX", "253", 1);
	setenv("DX", "252", 1);
	setenv("CA", "250", 1);

	read_hooks("/etc/pm/sleep.d", 2);
	read_hooks("/usr/lib/pm-utils/sleep.d", 1);
	sort_hooks();
	filter_hooks();

	if (argc == 2 && strcmp(argv[1], "print") == 0)
	{
		print_hooks(stderr);
		return 0;
	}

	int cur = 0;
	do
	{
		int canceled = 0;
		int inhibit = 0;
		fprintf(stderr, "Start of run from %d\n", cur);
		for ( ; cur < hooks_size && !canceled; ++cur)
		{
			int res = inhibit ? 0 : run_hook(cur, forwards);
			switch (res)
			{
				case 0: break;
				case -1: canceled = 1; break; // Cancel
				default: inhibit = 1; break;  // Inhibit
			}
		}
		if (canceled) --cur;

		if (cur == hooks_size)
		{
			do_suspend();
		}

		canceled = 0;
		for ( ; cur > 0 && !canceled; --cur)
		{
			int res = inhibit ? 0 : run_hook(cur-1, backwards);
			switch (res)
			{
				case 0: break;
				case -1: canceled = 1; break; // Cancel
				default: inhibit = 1; break;  // Inhibit
			}
		}
		if (canceled) ++cur;
		fprintf(stderr, "End of run, canceled: %d, inhibit: %d, cur: %d\n", canceled, inhibit, cur);
	} while (cur != 0);

	return 0;
}
