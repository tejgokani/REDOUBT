/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef REDOUBT_REPORT_H
#define REDOUBT_REPORT_H

#include <stdio.h>

#include "detector.h"
#include "finding.h"

/* Per-detector outcome recorded during a scan. */
struct detector_result {
	const char *name;
	enum detect_status status;
};

struct scan_report {
	const struct detector_result *results;
	size_t n_results;
	const struct finding_list *findings;
};

void report_text(FILE *out, const struct scan_report *r);
void report_json(FILE *out, const struct scan_report *r);

/* Writes s as a JSON string literal (with quotes) to out. Exposed for tests. */
void json_write_string(FILE *out, const char *s);

#endif
