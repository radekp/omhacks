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
#include <dlfcn.h>

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

typedef int hook_function(int argc, const char* argv[]);
typedef void hook_init_function(void);

struct hook {
	char* name;
	const char* dirname;
	int priority;
	int active;
	hook_function* function;
};

static struct hook hooks[MAX_HOOKS];
static int hooks_size = 0;
static const char *hooks_cur_dir = NULL;
static int hooks_cur_priority = 0;

void hooks_read_dynamic(const char* fname, int prio)
{
	hook_init_function* init = NULL;

	hooks_cur_dir = fname;
	hooks_cur_priority = prio;

	void* dl = dlopen(fname, RTLD_LAZY);
	if (dl == NULL) return;

	//*(void **) (&init) = dlsym(dl, "init");
	init = (hook_init_function*)dlsym(dl, "init");
	if (init == NULL)
	{
		dlclose(dl);
		return;
	}
	init();
}

static int hooks_is_dynamic(const char* fname)
{
	int len = strlen(fname);
	if (len < 3) return 0;
	return strcmp(fname + len - 3, ".so") == 0;
}

static int hooks_read_dir(const char* dir, int priority)
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
		if (hooks_size >= MAX_HOOKS) break;
		if (d->d_name[0] == '.') continue;
		snprintf(fullname, PATH_MAX, "%s/%s", dir, d->d_name);
		if (stat(fullname, &st) != 0) continue;
		// Skip directories
		if (S_ISDIR(st.st_mode)) continue;

		if (hooks_is_dynamic(d->d_name))
		{
			hooks_read_dynamic(fullname, priority+1);
		} else {
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
			hooks[hooks_size].function = NULL;
			++hooks_size;
		}
	}

	if (fdir != NULL) closedir(fdir);
	return res;
}

void hooks_add_function(const char* name, hook_function func)
{
	if (hooks_size >= MAX_HOOKS) return;

	hooks[hooks_size].name = strdup(name);
	if (hooks[hooks_size].name == NULL) goto error;
	hooks[hooks_size].dirname = strdup(hooks_cur_dir);
	hooks[hooks_size].priority = hooks_cur_priority;
	hooks[hooks_size].function = func;
	hooks[hooks_size].active = 1;

	++hooks_size;
	return;
error:
	if (hooks[hooks_size].name != NULL) free(hooks[hooks_size].name);
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

static void hooks_sort()
{
	qsort(hooks, hooks_size, sizeof(struct hook), hook_compare);
}

static int hooks_first_good(int start)
{
	do {
		// Look for the next active item
		while (start < hooks_size && !hooks[start].active)
			++start;
			
		// If we're the first item and active, we're good
		if (start == 0) return start;

		// Skip all items that have the same name as the previous one
		int i = start;
		while (i < hooks_size && strcmp(hooks[start-1].name, hooks[i].name) == 0)
			++i;
		start = i;
	} while (start < hooks_size && !hooks[start].active);
	return start;
}

static void hooks_filter()
{
	int src = 0;
	int dst = 0;
	while (src < hooks_size)
	{
		src = hooks_first_good(src);
		if (src >= hooks_size) break;
		if (src != dst)
			memcpy(&hooks[dst], &hooks[src], sizeof(struct hook));
		++dst;
		++src;
	}
	hooks_size = dst;
}

static void hooks_print(FILE* out)
{
	int i;
	fprintf(out, "Order pri fun active name                           dir\n");
	for (i = 0; i < hooks_size; ++i)
	{
		fprintf(out, "  %3i  %2i %3.3s    %3.3s %-30.30s %s\n",
			i, hooks[i].priority, hooks[i].function == NULL ? "no" : "yes",
			hooks[i].active ? "yes" : "no", hooks[i].name, hooks[i].dirname);
	}
}

static void hooks_read_all()
{
	timing_start();
	hooks_read_dir("/etc/pm/sleep.d", 10);
	fprintf(stderr, "Scanned /etc/pm/sleep.d in %ldusec\n", timing_end());

	timing_start();
	hooks_read_dir("/usr/lib/pm-utils/sleep.d", 0);
	fprintf(stderr, "Scanned /usr/lib/pm-utils/sleep.d in %ldusec\n", timing_end());
}

static int hook_run(int hook, const char* parm)
{
	char cmd[PATH_MAX + 30];
	int res;
    int argc = 2;
    const char* argv[3];

	if (!hooks[hook].active) return 0;
    argv[0] = hooks[hook].name;
    argv[1] = parm;
    argv[2] = NULL;

	timing_start();
	if (hooks[hook].function != NULL)
	{
		fprintf(stderr, "Running function %s:%s %s... ", hooks[hook].dirname, hooks[hook].name, parm);
		res = hooks[hook].function(argc, argv);
	} else {
		snprintf(cmd, PATH_MAX + 30, "%s/%s %s", hooks[hook].dirname, hooks[hook].name, parm);
		fprintf(stderr, "Running %s... ", cmd);
		res = system(cmd);
		if (!WIFEXITED(res))
			return res;
		res = WEXITSTATUS(res);
	}
	long int elapsed = timing_end();
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

/* Logging subsystem */

static FILE* log = NULL;



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

int setup_env(const char* logfile, const char* name)
{
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

    if (argc > 1)
    {
        // Setup the environment
        int res = setup_env(logfile, name);
        if (res != 0)
            return res;

        if (argc == 2 && strcmp(argv[1], "print") == 0)
        {
            hooks_read_all();
            hooks_sort();
            fprintf(stderr, "Before filtering:\n");
            hooks_print(stderr);
            hooks_filter();
            fprintf(stderr, "After filtering:\n");
            hooks_print(stderr);
            return 0;
        }

        if (argc > 2 && strcmp(argv[1], "run") == 0)
        {
            // pm-suspend run <script> param
            int cur = 0;
            hooks_read_all();
            hooks_sort();
            hooks_filter();
            for ( ; cur < hooks_size; ++cur)
                if (strcmp(hooks[cur].name, argv[2]) == 0)
                {
                    if (argc > 3)
                        return hook_run(cur, argv[3]);
                    else
                    {
                        int res = hook_run(cur, forwards);
                        printf("%s %s returned status %d\n", argv[2], forwards, res);
                        res = hook_run(cur, backwards);
                        printf("%s %s returned status %d\n", argv[2], backwards, res);
                        return res;
                    }
                }
            fprintf(stderr, "Hook %s not found\n", argv[2]);
            return 1;
        }

        fprintf(stderr, "Usage: %s\n", argv[0]);
        fprintf(stderr, "Usage: %s print\n", argv[0]);
        fprintf(stderr, "Usage: %s run <hookname> <param>\n", argv[0]);
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
    int res = setup_env(logfile, name);
    if (res != 0)
        return res;

	hooks_read_all();
	hooks_sort();
	hooks_filter();

	int cur = 0;
	do
	{
		int canceled = 0;
		int inhibit = 0;
		fprintf(stderr, "Start of run from %d\n", cur);
		for ( ; cur < hooks_size && !canceled; ++cur)
		{
			int res = inhibit ? 0 : hook_run(cur, forwards);
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
			int res = inhibit ? 0 : hook_run(cur-1, backwards);
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
