#include <stdio.h>
#include <stdlib.h>

void test();

void restart() {}

int main(int argc, char **argv)
{
	test();
	const char *const test_name = argv[0];
	printf("[ OK ] %s\n", test_name);
	exit(EXIT_SUCCESS);
}
