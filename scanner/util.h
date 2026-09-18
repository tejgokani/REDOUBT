/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef REDOUBT_UTIL_H
#define REDOUBT_UTIL_H

#include <stddef.h>

/* Growable list of owned strings. Zero-initialise before use. */
struct str_list {
	char **items;
	size_t len;
	size_t cap;
};

/* Copies s. Returns 0 or -1 (ENOMEM). */
int str_list_push(struct str_list *l, const char *s);
int str_list_contains(const struct str_list *l, const char *s);
void str_list_free(struct str_list *l);

/* Reads a whole file into a NUL-terminated malloc'd buffer. Returns 0 or -errno. */
int read_file(const char *path, char **out, size_t *len);

#endif
