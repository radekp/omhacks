#include <string.h>

typedef int hook_function(int argc, const char* argv[]);
extern void hooks_add_function(const char* name, hook_function func);

static int hook_at(int argc, const char* argv[])
{
    if (argc < 2) return 254;

    if (strcmp(argv[1], "suspend") == 0)
    {
        return 0;
    }
    else if (strcmp(argv[1], "resume") == 0)
    {
        return 0;
    }

    return 254;
}

void init()
{
    hooks_add_function("00at", hook_at);
}
