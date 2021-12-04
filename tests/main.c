#include <stdio.h>

int test();

void restart() {}

int main(int argc, char **argv)
{
	const char *const test_name = argv[0];
	const int test_result = test();

	if (test_result == 0) {
		printf("[ OK ] %s\n", test_name);
	} else {
		printf("[FAIL] %s\n", test_name);
	}

	return test_result;
}
