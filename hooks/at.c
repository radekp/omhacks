#include <string.h>

typedef int hook_function(int argc, const char* argv[]);
extern void hooks_add_function(const char* name, hook_function func);

static time_t first_in_at_queue()
{
    time_t first = 0;
#if 0
        // TODO
    atq = subprocess.Popen(["atq"], stdout=subprocess.PIPE).communicate()[0]
    for line in atq.split("\n"):
        if not line: continue
        qid, rest = line.split("\t")
        date, queue, user = rest.rsplit(' ', 2)
        t = time.mktime(time.strptime(date, "%a %b %d %H:%M:%S %Y"))
        if first is None:
            first = t
        elif t < first:
            first = t
#endif
    return first
}

static int hook_at(int argc, const char* argv[])
{
    if (argc < 2) return 254;

    if (strcmp(argv[1], "suspend") == 0)
    {
        time_t first = first_in_at_queue();
        if (first != 0)
        {
            time_t now = time(NULL);
            // Wake up 2 seconds after, so that atd -s runs pending jobs right away
            int delay = first - now - 2;
            if (delay < 15) delay = 15;
#if 0
            // TODO
            subprocess.check_call(["rtcwake", "-m", "no", "-s", str(int(delay))])
            print "Wakeup scheduled in %d seconds" % delay
#endif
        }
        return 0;
    }
    else if (strcmp(argv[1], "resume") == 0)
    {
#if 0
STORAGEDIR="%s/%s/storage" % (os.environ["PM_UTILS_RUNDIR"], os.environ["STASHNAME"])
        // TODO
        # Touch last-resume flagfile
        open(STORAGEDIR+"/last-resume", "w").close()

        if subprocess.call(["om", "resume-reason", "contains", "EINT09_PMU:rtc_alarm"]) == 0:
            print "Running atd -s"
            subprocess.check-call(["atd", "-s"])
#endif
        return 0;
    }

    return 254;
}

void init()
{
    hooks_add_function("00at", hook_at);
}
