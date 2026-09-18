// SPDX-License-Identifier: GPL-2.0-only
#include "source.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

int proc_parse_modules(const char *text, struct str_list *out)
{
	const char *p = text;

	while (*p) {
		const char *end = p;
		char name[256];
		size_t n;

		while (*end && *end != ' ' && *end != '\n')
			end++;
		n = (size_t)(end - p);
		if (n > 0 && n < sizeof(name)) {
			memcpy(name, p, n);
			name[n] = '\0';
			if (str_list_push(out, name) != 0)
				return -ENOMEM;
		}
		while (*end && *end != '\n')
			end++;
		p = *end ? end + 1 : end;
	}
	return 0;
}

#ifdef __linux__
static int proc_probe(const struct source *self)
{
	char *buf;
	int rc = read_file("/proc/modules", &buf, NULL);

	(void)self;
	if (rc == 0)
		free(buf);
	return rc;
}

static int proc_list_modules(const struct source *self, struct str_list *out)
{
	char *buf;
	int rc = read_file("/proc/modules", &buf, NULL);

	(void)self;
	if (rc)
		return rc;
	rc = proc_parse_modules(buf, out);
	free(buf);
	return rc;
}
#else
static int proc_probe(const struct source *self)
{
	(void)self;
	return -ENOSYS;
}

static int proc_list_modules(const struct source *self, struct str_list *out)
{
	(void)self;
	(void)out;
	return -ENOSYS;
}
#endif

const struct source source_proc = {
    .name = "proc",
    .probe = proc_probe,
    .list_modules = proc_list_modules,
};
