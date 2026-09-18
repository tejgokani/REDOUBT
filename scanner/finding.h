/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef REDOUBT_FINDING_H
#define REDOUBT_FINDING_H

#include <stddef.h>

enum severity {
	SEV_INFO = 0,
	SEV_LOW,
	SEV_MEDIUM,
	SEV_HIGH,
	SEV_CRITICAL,
};

const char *severity_name(enum severity s);

#define FINDING_MAX_EVIDENCE 8

struct finding {
	char *id;	/* stable machine id, e.g. "hidden-module.sysfs-only" */
	char *detector; /* detector name that produced it */
	char *message;	/* one-line human description */
	enum severity severity;
	char *evidence[FINDING_MAX_EVIDENCE]; /* "key=value" strings */
	size_t n_evidence;
};

struct finding_list {
	struct finding *items;
	size_t len;
	size_t cap;
};

/* Appends a finding (all strings copied). Returns index or -1 on ENOMEM. */
int finding_list_add(struct finding_list *l, const char *detector, const char *id,
		     enum severity sev, const char *message);
/* Attaches evidence to finding at idx. Returns 0, or -1 if full/ENOMEM. */
int finding_add_evidence(struct finding_list *l, size_t idx, const char *evidence);
/* Highest severity present, or SEV_INFO if empty. */
enum severity finding_list_max_severity(const struct finding_list *l);
void finding_list_free(struct finding_list *l);

#endif
