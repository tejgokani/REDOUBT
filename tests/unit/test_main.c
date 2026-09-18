// SPDX-License-Identifier: GPL-2.0-only
#include "test_runner.h"

int t_checks, t_failed;

int main(void)
{
	test_util();
	test_finding();
	test_detector();
	test_report();
	test_source_proc();
	printf("%d checks, %d failed\n", t_checks, t_failed);
	return t_failed ? 1 : 0;
}
