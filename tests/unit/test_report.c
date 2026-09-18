// SPDX-License-Identifier: GPL-2.0-only
#include <stdio.h>
#include <stdlib.h>

#include "../../scanner/report.h"
#include "test_runner.h"

static char *capture(void (*fn)(FILE *, const struct scan_report *), const struct scan_report *r)
{
	FILE *f = tmpfile();
	long n;
	char *buf;

	fn(f, r);
	n = ftell(f);
	rewind(f);
	buf = calloc((size_t)n + 1, 1);
	if (fread(buf, 1, (size_t)n, f) != (size_t)n)
		buf[0] = '\0';
	fclose(f);
	return buf;
}

void test_report(void)
{
	struct finding_list l = { 0 };
	struct detector_result res[] = { { "taint", DETECT_OK } };
	struct scan_report r = { res, 1, &l };
	char *out;

	out = capture(report_json, &r);
	CHECK_STR(out, "{\"schema\":1,\"detectors\":[{\"name\":\"taint\",\"status\":\"ok\"}],\"findings\":[]}\n");
	free(out);

	finding_list_add(&l, "taint", "taint.unsigned", SEV_MEDIUM, "say \"hi\"\n");
	finding_add_evidence(&l, 0, "flags=0x1");
	out = capture(report_json, &r);
	CHECK(strstr(out, "\"severity\":\"medium\"") != NULL);
	CHECK(strstr(out, "say \\\"hi\\\"\\n") != NULL); /* escaped quotes/newline */
	CHECK(strstr(out, "\"evidence\":[\"flags=0x1\"]") != NULL);
	free(out);

	out = capture(report_text, &r);
	CHECK(strstr(out, "1 finding(s)") != NULL);
	free(out);
	finding_list_free(&l);
}
