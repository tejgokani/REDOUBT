/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef REDOUBT_DETECTOR_H
#define REDOUBT_DETECTOR_H

#include <stddef.h>

#include "finding.h"
#include "source.h"

struct scan_ctx {
	const struct source *proc;
	const struct source *sysfs;
	const struct source *kmod; /* may be NULL / unavailable: degrade gracefully */
};

enum detect_status {
	DETECT_OK = 0,
	DETECT_NOT_IMPLEMENTED = 1, /* registered but no logic yet */
	DETECT_UNAVAILABLE = 2,     /* required source missing (e.g. no kmod) */
	DETECT_ERROR = 3,
};

struct detector {
	const char *name;        /* CLI id: --check <name> */
	const char *description;
	int needs_kmod;          /* 1 if it cannot run without /dev/redoubt */
	enum detect_status (*run)(const struct scan_ctx *ctx, struct finding_list *out);
};

/* Registry: static table defined in detector.c. */
size_t detector_count(void);
const struct detector *detector_at(size_t i);
const struct detector *detector_find(const char *name);

const char *detect_status_name(enum detect_status s);

#endif
