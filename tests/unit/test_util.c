// SPDX-License-Identifier: GPL-2.0-only
#include "../../scanner/util.h"
#include "test_runner.h"

void test_util(void)
{
	struct str_list l = {0};

	CHECK(str_list_push(&l, "a") == 0);
	for (int i = 0; i < 20; i++) /* forces growth past initial capacity */
		CHECK(str_list_push(&l, "b") == 0);
	CHECK(l.len == 21);
	CHECK(str_list_contains(&l, "a"));
	CHECK(!str_list_contains(&l, "zzz"));
	str_list_free(&l);
	CHECK(l.len == 0 && l.items == NULL);

	char *buf = NULL;

	CHECK(read_file("/nonexistent/redoubt", &buf, NULL) < 0);
}
