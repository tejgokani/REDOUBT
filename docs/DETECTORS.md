# Detector specifications
Names here must match `scanner/detector.c` and `tests/unit/test_detector.c`. Finding ids are stable.
Status: all are **stubs** after Session 1.

## hidden-module (Session 2) — needs kmod
- **Views:** (A) `/proc/modules`; (B) `/sys/module/*` dirs that have `initstate`; (C) kmod: kernel `modules` list walk; (D) kmod: module address ranges from `mod_tree`/kobject list.
- **Algorithm:** build name sets per view; any name in some views but not others → finding.
- **Findings:** `hidden-module.unlinked` (in D/C-independent view, missing from A) — critical; `hidden-module.sysfs-only` — high; `hidden-module.proc-only` — medium (race/unload, re-check once).
- **False positives:** module load/unload racing the scan → re-read once before reporting; built-in modules in sysfs without `initstate` are excluded.
- **Tests:** mock sources with a name missing in one view; testrk unlinking itself.

## syscall-table (Session 2) — needs kmod
- **Input:** kmod `READ_SYSCALL_TABLE` (N pointers) + `GET_TEXT_RANGE` (`_stext.._etext`) + module ranges.
- **Algorithm:** entry outside kernel text → in a module range? report owning module : unknown region.
- **Findings:** `syscall-table.outside-text` — critical; `syscall-table.in-module` — high.
- **FP:** arch wrappers/CFI stubs (aarch64/x86 differ) — compare against text range, not symbol names.
- **Note:** kernels ≥ 6.9 may hide `sys_call_table`; resolve via kprobe trick (KERNEL_COMPAT.md); report `unavailable` if unresolvable.

## hidden-process (Session 2) — needs kmod
- **Views:** kernel task walk (`for_each_process`) vs `/proc` readdir vs `kill(pid,0)`/`/proc/<pid>` stat probe over pid range.
- **Findings:** `hidden-process.not-in-proc` — high. Re-check to avoid exit races.

## taint (Session 2) — no kmod
- **Input:** `/proc/sys/kernel/tainted` bits (proprietary P, forced F, unsigned E, out-of-tree O) and `/sys/module/*/taint`.
- **Findings:** `taint.unsigned-module` — medium; `taint.out-of-tree` — low; `taint.forced-load` — medium.

## ftrace-hooks (Session 3) — needs kmod
- **Input:** kmod `LIST_FTRACE` (`ftrace_ops` callbacks, kprobes) + `/sys/kernel/tracing/{enabled_functions,kprobe_events}`.
- **Findings:** `ftrace-hooks.foreign-callback` — high (callback in a module not on allowlist), `ftrace-hooks.on-syscall` — high.
- **FP:** eBPF, livepatch, perf → allowlist.

## hidden-file (Session 3) — no kmod
- **Algorithm:** compare `readdir` of a directory to `stat`/`open` probes of names from a second source (e.g. a wordlist of known rootkit filenames, `/proc/kallsyms`, module names, dentries via `find`-style vs `openat` by name).
- **Findings:** `hidden-file.stat-only` — high.

## net-hooks (Session 3) — needs kmod
- **Input:** kmod list of netfilter hooks; `/proc/kallsyms` symbol count/consistency checks.
- **Findings:** `net-hooks.foreign-hook` — medium; `net-hooks.kallsyms-mismatch` — medium.
