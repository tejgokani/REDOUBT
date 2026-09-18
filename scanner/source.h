/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef REDOUBT_SOURCE_H
#define REDOUBT_SOURCE_H

#include "util.h"

/*
 * A "source" is one independent view of system state (procfs, sysfs, the
 * Redoubt kmod, or a test mock). Detectors compare views from different
 * sources; they never read the system directly so they stay unit-testable.
 *
 * Every op is optional (NULL == unsupported). Ops return 0 on success or
 * -errno; -ENOSYS means "this source cannot provide that view".
 * Session 2 adds ops (tasks, syscall table, text range) as detectors need them.
 */
struct source {
	const char *name;
	/* 0 if the source can be used on this system. */
	int (*probe)(const struct source *self);
	/* Names of loaded kernel modules as this source sees them. */
	int (*list_modules)(const struct source *self, struct str_list *out);
	void *priv; /* source-specific context (e.g. mock data) */
};

/* /proc based view. Linux only; other platforms return -ENOSYS. */
extern const struct source source_proc;
/* /sys/module based view. Linux only. */
extern const struct source source_sysfs;
/* /dev/redoubt view (kmod ground truth). Linux only. */
extern const struct source source_kmod;

/* Parses the text of /proc/modules, pushing the first column of each line.
 * Exposed separately from source_proc so it is unit-testable. */
int proc_parse_modules(const char *text, struct str_list *out);

#endif
