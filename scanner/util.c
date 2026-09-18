// SPDX-License-Identifier: GPL-2.0-only
#include "util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int str_list_push(struct str_list *l, const char *s)
{
	char *copy = strdup(s);

	if (!copy)
		return -1;
	if (l->len == l->cap) {
		size_t ncap = l->cap ? l->cap * 2 : 8;
		char **n = realloc(l->items, ncap * sizeof(*n));

		if (!n) {
			free(copy);
			return -1;
		}
		l->items = n;
		l->cap = ncap;
	}
	l->items[l->len++] = copy;
	return 0;
}

int str_list_contains(const struct str_list *l, const char *s)
{
	for (size_t i = 0; i < l->len; i++)
		if (strcmp(l->items[i], s) == 0)
			return 1;
	return 0;
}

void str_list_free(struct str_list *l)
{
	for (size_t i = 0; i < l->len; i++)
		free(l->items[i]);
	free(l->items);
	l->items = NULL;
	l->len = l->cap = 0;
}

int read_file(const char *path, char **out, size_t *len)
{
	FILE *f = fopen(path, "r");
	size_t cap = 4096, n = 0;
	char *buf;

	if (!f)
		return -errno;
	buf = malloc(cap);
	if (!buf) {
		fclose(f);
		return -ENOMEM;
	}
	for (;;) {
		size_t got = fread(buf + n, 1, cap - n - 1, f);

		n += got;
		if (n < cap - 1)
			break; /* EOF or error */
		cap *= 2;
		char *nb = realloc(buf, cap);

		if (!nb) {
			free(buf);
			fclose(f);
			return -ENOMEM;
		}
		buf = nb;
	}
	if (ferror(f)) {
		free(buf);
		fclose(f);
		return -EIO;
	}
	fclose(f);
	buf[n] = '\0';
	*out = buf;
	if (len)
		*len = n;
	return 0;
}
