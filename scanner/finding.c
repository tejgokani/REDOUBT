// SPDX-License-Identifier: GPL-2.0-only
#include "finding.h"

#include <stdlib.h>
#include <string.h>

const char *severity_name(enum severity s)
{
	switch (s) {
	case SEV_INFO:
		return "info";
	case SEV_LOW:
		return "low";
	case SEV_MEDIUM:
		return "medium";
	case SEV_HIGH:
		return "high";
	case SEV_CRITICAL:
		return "critical";
	}
	return "unknown";
}

int finding_list_add(struct finding_list *l, const char *detector, const char *id,
		     enum severity sev, const char *message)
{
	struct finding *f;

	if (l->len == l->cap) {
		size_t ncap = l->cap ? l->cap * 2 : 8;
		struct finding *n = realloc(l->items, ncap * sizeof(*n));

		if (!n)
			return -1;
		l->items = n;
		l->cap = ncap;
	}
	f = &l->items[l->len];
	memset(f, 0, sizeof(*f));
	f->detector = strdup(detector);
	f->id = strdup(id);
	f->message = strdup(message);
	f->severity = sev;
	if (!f->detector || !f->id || !f->message) {
		free(f->detector);
		free(f->id);
		free(f->message);
		return -1;
	}
	return (int)l->len++;
}

int finding_add_evidence(struct finding_list *l, size_t idx, const char *evidence)
{
	struct finding *f;

	if (idx >= l->len)
		return -1;
	f = &l->items[idx];
	if (f->n_evidence >= FINDING_MAX_EVIDENCE)
		return -1;
	f->evidence[f->n_evidence] = strdup(evidence);
	if (!f->evidence[f->n_evidence])
		return -1;
	f->n_evidence++;
	return 0;
}

enum severity finding_list_max_severity(const struct finding_list *l)
{
	enum severity max = SEV_INFO;

	for (size_t i = 0; i < l->len; i++)
		if (l->items[i].severity > max)
			max = l->items[i].severity;
	return max;
}

void finding_list_free(struct finding_list *l)
{
	for (size_t i = 0; i < l->len; i++) {
		struct finding *f = &l->items[i];

		free(f->id);
		free(f->detector);
		free(f->message);
		for (size_t j = 0; j < f->n_evidence; j++)
			free(f->evidence[j]);
	}
	free(l->items);
	l->items = NULL;
	l->len = l->cap = 0;
}
