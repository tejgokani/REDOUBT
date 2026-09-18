// SPDX-License-Identifier: GPL-2.0-only
#include "../../scanner/detector.h"
#include "test_runner.h"

void test_detector(void)
{
	/* Names must match docs/DETECTORS.md. */
	static const char *expected[] = {"hidden-module", "syscall-table", "hidden-process",
					 "taint",	  "ftrace-hooks",  "hidden-file",
					 "net-hooks"};
	size_t n = sizeof(expected) / sizeof(expected[0]);

	CHECK(detector_count() == n);
	for (size_t i = 0; i < n; i++)
		CHECK(detector_find(expected[i]) != NULL);
	CHECK(detector_find("nope") == NULL);
	CHECK(detector_at(detector_count()) == NULL);

	struct scan_ctx ctx = {0};
	struct finding_list out = {0};

	/* Session 1: all stubs report not-implemented and add no findings. */
	CHECK(detector_find("taint")->run(&ctx, &out) == DETECT_NOT_IMPLEMENTED);
	CHECK(out.len == 0);
}
