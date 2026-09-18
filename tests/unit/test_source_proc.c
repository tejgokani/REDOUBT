// SPDX-License-Identifier: GPL-2.0-only
#include "../../scanner/source.h"
#include "test_runner.h"

void test_source_proc(void)
{
	static const char text[] = "ext4 1032192 1 - Live 0x0000000000000000\n"
				   "evil_mod 16384 0 - Live 0x0000000000000000 (OE)\n"
				   "\n"
				   "last 4096 0 - Live 0x0";
	struct str_list l = {0};

	CHECK(proc_parse_modules(text, &l) == 0);
	CHECK(l.len == 3);
	CHECK(str_list_contains(&l, "ext4"));
	CHECK(str_list_contains(&l, "evil_mod"));
	CHECK(str_list_contains(&l, "last")); /* no trailing newline */
	str_list_free(&l);

	CHECK(proc_parse_modules("", &l) == 0 && l.len == 0);
}
