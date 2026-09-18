// SPDX-License-Identifier: GPL-2.0-only
#include "detector.h"

#include <string.h>

/*
 * Session 1: every detector is a registered stub. Session 2/3 replace the stub
 * `run` functions with real logic (one file per detector under scanner/detectors/).
 * Keep this table in sync with docs/DETECTORS.md.
 */
static enum detect_status stub_run(const struct scan_ctx *ctx, struct finding_list *out)
{
	(void)ctx;
	(void)out;
	return DETECT_NOT_IMPLEMENTED;
}

static const struct detector registry[] = {
	{ "hidden-module", "Loaded modules present in one kernel view but not another", 1, stub_run },
	{ "syscall-table", "System call entries pointing outside kernel text", 1, stub_run },
	{ "hidden-process", "Tasks visible to the kernel but absent from /proc", 1, stub_run },
	{ "taint", "Kernel taint flags indicating unsigned/out-of-tree modules", 0, stub_run },
	{ "ftrace-hooks", "ftrace/kprobe hooks not owned by known tooling", 1, stub_run },
	{ "hidden-file", "Directory entries hidden from getdents but reachable by name", 0, stub_run },
	{ "net-hooks", "Suspicious netfilter hooks and kallsyms inconsistencies", 1, stub_run },
};

size_t detector_count(void)
{
	return sizeof(registry) / sizeof(registry[0]);
}

const struct detector *detector_at(size_t i)
{
	return i < detector_count() ? &registry[i] : NULL;
}

const struct detector *detector_find(const char *name)
{
	for (size_t i = 0; i < detector_count(); i++)
		if (strcmp(registry[i].name, name) == 0)
			return &registry[i];
	return NULL;
}

const char *detect_status_name(enum detect_status s)
{
	switch (s) {
	case DETECT_OK:
		return "ok";
	case DETECT_NOT_IMPLEMENTED:
		return "not-implemented";
	case DETECT_UNAVAILABLE:
		return "unavailable";
	case DETECT_ERROR:
		return "error";
	}
	return "unknown";
}
