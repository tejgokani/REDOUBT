/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef REDOUBT_TEST_RUNNER_H
#define REDOUBT_TEST_RUNNER_H

#include <stdio.h>
#include <string.h>

extern int t_checks, t_failed;

#define CHECK(cond)                                                                                \
	do {                                                                                       \
		t_checks++;                                                                        \
		if (!(cond)) {                                                                     \
			t_failed++;                                                                \
			fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);            \
		}                                                                                  \
	} while (0)

#define CHECK_STR(a, b) CHECK(strcmp((a), (b)) == 0)

/* Each test file exposes one suite entry point; add it in test_main.c. */
void test_util(void);
void test_finding(void);
void test_detector(void);
void test_report(void);
void test_source_proc(void);

#endif
