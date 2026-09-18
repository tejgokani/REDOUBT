// SPDX-License-Identifier: GPL-2.0-only
#include "../../scanner/finding.h"
#include "test_runner.h"

void test_finding(void)
{
	struct finding_list l = { 0 };

	CHECK(finding_list_max_severity(&l) == SEV_INFO);
	int a = finding_list_add(&l, "hidden-module", "hidden-module.x", SEV_HIGH, "msg");

	CHECK(a == 0);
	CHECK(finding_add_evidence(&l, 0, "module=evil") == 0);
	CHECK(finding_add_evidence(&l, 5, "bad index") == -1);
	for (int i = 1; i < FINDING_MAX_EVIDENCE; i++)
		CHECK(finding_add_evidence(&l, 0, "k=v") == 0);
	CHECK(finding_add_evidence(&l, 0, "overflow") == -1);
	CHECK(finding_list_add(&l, "d", "i", SEV_CRITICAL, "m") == 1);
	CHECK(finding_list_max_severity(&l) == SEV_CRITICAL);
	CHECK_STR(severity_name(SEV_MEDIUM), "medium");
	finding_list_free(&l);
	CHECK(l.len == 0);
}
