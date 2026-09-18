// SPDX-License-Identifier: GPL-2.0-only
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "detector.h"
#include "report.h"
#include "source.h"

#ifdef __linux__
#include "redoubt_abi.h"
int kmod_get_version(struct rdt_version *v);
#endif

#define REDOUBT_VERSION "0.1.0"

/* Exit codes (docs/ARCHITECTURE.md): 0 clean, 1 findings, 2 error/usage. */
enum { EXIT_CLEAN = 0, EXIT_FINDINGS = 1, EXIT_FAILURE_CODE = 2 };

static void usage(FILE *out)
{
	fputs("Usage: redoubt <command> [options]\n"
	      "\n"
	      "Commands:\n"
	      "  scan [--check NAME]... [--json]   Run detectors (default: all)\n"
	      "  list-checks                       List available detectors\n"
	      "  doctor                            Check environment and kmod status\n"
	      "  version                           Print version\n",
	      out);
}

static int cmd_list_checks(void)
{
	for (size_t i = 0; i < detector_count(); i++) {
		const struct detector *d = detector_at(i);

		printf("%-16s %s%s\n", d->name, d->description,
		       d->needs_kmod ? " [needs kmod]" : "");
	}
	return EXIT_CLEAN;
}

static int cmd_doctor(void)
{
	int rc;

	printf("redoubt %s\n", REDOUBT_VERSION);
#ifdef __linux__
	printf("platform : linux\n");
#else
	printf("platform : non-linux (detectors unavailable; unit-test build only)\n");
#endif
	printf("proc     : %s\n", source_proc.probe(&source_proc) == 0 ? "ok" : "unavailable");
	printf("sysfs    : %s\n", source_sysfs.probe(&source_sysfs) == 0 ? "ok" : "unavailable");
	rc = source_kmod.probe(&source_kmod);
	if (rc == 0) {
#ifdef __linux__
		struct rdt_version v;

		if (kmod_get_version(&v) == 0)
			printf("kmod     : ok (abi %u.%u, %s)\n", v.abi_major, v.abi_minor,
			       v.abi_major == RDT_ABI_MAJOR ? "compatible" : "INCOMPATIBLE");
#endif
	} else {
		printf("kmod     : unavailable (%s) - reduced coverage\n", strerror(-rc));
	}
	return EXIT_CLEAN;
}

static int cmd_scan(int argc, char **argv)
{
	const struct detector *selected[64];
	size_t n_selected = 0;
	int json = 0;
	struct scan_ctx ctx = {&source_proc, &source_sysfs, &source_kmod};
	struct finding_list findings = {0};
	struct detector_result results[64];
	struct scan_report report;
	int kmod_ok = source_kmod.probe(&source_kmod) == 0;

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--json") == 0) {
			json = 1;
		} else if (strcmp(argv[i], "--check") == 0 && i + 1 < argc) {
			const struct detector *d = detector_find(argv[++i]);

			if (!d) {
				fprintf(stderr, "redoubt: unknown check '%s'\n", argv[i]);
				return EXIT_FAILURE_CODE;
			}
			if (n_selected < 64)
				selected[n_selected++] = d;
		} else {
			fprintf(stderr, "redoubt: bad option '%s'\n", argv[i]);
			return EXIT_FAILURE_CODE;
		}
	}
	if (n_selected == 0)
		for (size_t i = 0; i < detector_count() && n_selected < 64; i++)
			selected[n_selected++] = detector_at(i);

	for (size_t i = 0; i < n_selected; i++) {
		const struct detector *d = selected[i];

		results[i].name = d->name;
		if (d->needs_kmod && !kmod_ok)
			results[i].status = DETECT_UNAVAILABLE;
		else
			results[i].status = d->run(&ctx, &findings);
	}

	report.results = results;
	report.n_results = n_selected;
	report.findings = &findings;
	if (json)
		report_json(stdout, &report);
	else
		report_text(stdout, &report);

	int code = findings.len ? EXIT_FINDINGS : EXIT_CLEAN;

	for (size_t i = 0; i < n_selected; i++)
		if (results[i].status == DETECT_ERROR)
			code = EXIT_FAILURE_CODE;
	finding_list_free(&findings);
	return code;
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		usage(stderr);
		return EXIT_FAILURE_CODE;
	}
	if (strcmp(argv[1], "scan") == 0)
		return cmd_scan(argc - 2, argv + 2);
	if (strcmp(argv[1], "list-checks") == 0)
		return cmd_list_checks();
	if (strcmp(argv[1], "doctor") == 0)
		return cmd_doctor();
	if (strcmp(argv[1], "version") == 0 || strcmp(argv[1], "--version") == 0) {
		printf("redoubt %s\n", REDOUBT_VERSION);
		return EXIT_CLEAN;
	}
	if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "help") == 0) {
		usage(stdout);
		return EXIT_CLEAN;
	}
	usage(stderr);
	return EXIT_FAILURE_CODE;
}
