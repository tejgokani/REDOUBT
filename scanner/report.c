// SPDX-License-Identifier: GPL-2.0-only
#include "report.h"

void json_write_string(FILE *out, const char *s)
{
	fputc('"', out);
	for (; *s; s++) {
		unsigned char c = (unsigned char)*s;

		switch (c) {
		case '"':
			fputs("\\\"", out);
			break;
		case '\\':
			fputs("\\\\", out);
			break;
		case '\n':
			fputs("\\n", out);
			break;
		case '\r':
			fputs("\\r", out);
			break;
		case '\t':
			fputs("\\t", out);
			break;
		default:
			if (c < 0x20)
				fprintf(out, "\\u%04x", c);
			else
				fputc(c, out);
		}
	}
	fputc('"', out);
}

void report_text(FILE *out, const struct scan_report *r)
{
	fprintf(out, "Redoubt scan\n");
	for (size_t i = 0; i < r->n_results; i++)
		fprintf(out, "  [%s] %s\n", detect_status_name(r->results[i].status),
			r->results[i].name);
	if (r->findings->len == 0) {
		fprintf(out, "No findings.\n");
		return;
	}
	fprintf(out, "%zu finding(s):\n", r->findings->len);
	for (size_t i = 0; i < r->findings->len; i++) {
		const struct finding *f = &r->findings->items[i];

		fprintf(out, "  %-8s %s (%s): %s\n", severity_name(f->severity), f->id,
			f->detector, f->message);
		for (size_t j = 0; j < f->n_evidence; j++)
			fprintf(out, "           %s\n", f->evidence[j]);
	}
}

void report_json(FILE *out, const struct scan_report *r)
{
	fputs("{\"schema\":1,\"detectors\":[", out);
	for (size_t i = 0; i < r->n_results; i++) {
		if (i)
			fputc(',', out);
		fputs("{\"name\":", out);
		json_write_string(out, r->results[i].name);
		fputs(",\"status\":", out);
		json_write_string(out, detect_status_name(r->results[i].status));
		fputc('}', out);
	}
	fputs("],\"findings\":[", out);
	for (size_t i = 0; i < r->findings->len; i++) {
		const struct finding *f = &r->findings->items[i];

		if (i)
			fputc(',', out);
		fputs("{\"id\":", out);
		json_write_string(out, f->id);
		fputs(",\"detector\":", out);
		json_write_string(out, f->detector);
		fputs(",\"severity\":", out);
		json_write_string(out, severity_name(f->severity));
		fputs(",\"message\":", out);
		json_write_string(out, f->message);
		fputs(",\"evidence\":[", out);
		for (size_t j = 0; j < f->n_evidence; j++) {
			if (j)
				fputc(',', out);
			json_write_string(out, f->evidence[j]);
		}
		fputs("]}", out);
	}
	fputs("]}\n", out);
}
