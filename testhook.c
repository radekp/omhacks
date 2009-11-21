#include <stdio.h>

typedef int hook_function(const char* name, const char* param);
extern void hooks_add_function(const char* name, hook_function func);

static int test_hook(const char* name, const char* param)
{
	fprintf(stderr, "test_hook action %s %s\n", name, param);
	return 0;
}

void init()
{
	fprintf(stderr, "INIT\n");
	hooks_add_function("50-test1", test_hook);
	hooks_add_function("30-test2", test_hook);
}
