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
#include <errno.h>

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
		if (strcmp(hooks[last].name, hooks[i].name) == 0)
			continue;
		if (last + 1 != i)
			memcpy(&hooks[last+1], &hooks[i], sizeof(struct hook));
		++last;
	}
	hooks_size = last;
}

static void print_hooks()
{
	int i;
	for (i = 0; i < hooks_size; ++i)
	{
		printf("%3i: %s/%s\n", i+1, hooks[i].dirname, hooks[i].name);
	}
}

static int run_hook(int hook, const char* parm)
{
	char cmd[PATH_MAX + 30];
	int res;
	if (!hooks[hook].active) return 0;

	snprintf(cmd, PATH_MAX + 30, "%s/%s %s", hooks[hook].dirname, hooks[hook].name, parm);
	printf("Running %s... ", cmd);
	res = system(cmd);
	if (!WIFEXITED(res))
		return res;
	res = WEXITSTATUS(res);
	printf("result: %d\n", res);
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
	setenv("PM_FUNCTIONS", "/usr/lib/pm-utils/pm-functions", 1);
	setenv("PM_LOGFILE", logfile, 1);
	setenv("STASHNAME", name, 1);
	setenv("NA", "254", 1);
	setenv("NX", "253", 1);
	setenv("DX", "252", 1);

	read_hooks("/etc/pm/sleep.d", 2);
	read_hooks("/usr/lib/pm-utils/sleep.d", 1);
	sort_hooks();
	filter_hooks();
	// print_hooks();

	int cur = 0;
	do
	{
		int canceled = 0;
		int inhibit = 0;
		for ( ; cur < hooks_size && !canceled; ++cur)
			switch (inhibit || run_hook(cur, forwards))
			{
				case 0: break;
				case -1: canceled = 1; break; // Cancel
				default: inhibit = 1; break;  // Inhibit
			}

		if (cur == hooks_size)
		{
			do_suspend();
		}

		canceled = 0;
		for ( ; cur > 0 && !canceled; --cur)
			switch (inhibit || run_hook(cur-1, backwards))
			{
				case 0: break;
				case -1: canceled = 1; break; // Cancel
				default: inhibit = 1; break;  // Inhibit
			}
	} while (cur != 0);

	return 0;
}
