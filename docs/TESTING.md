# Testing strategy

## Layers
1. **Unit (host, `make test`)** — `tests/unit/`, zero-dependency runner (`CHECK`, `CHECK_STR`). Detectors are tested against **mock sources** (`struct source` with fixture data) — no kernel needed. Add a suite function to `test_runner.h` + `test_main.c` per new test file.
2. **Integration (VM, `tools/vm-test.sh`)** — `tests/integration/*.sh`: load kmod, load a `testrk` fixture, run `redoubt scan --json`, assert expected finding ids, unload, assert clean `dmesg`.
3. **Negative/clean baseline** — a stock VM must produce **zero** findings (false-positive guard). Run on every integration pass.
4. **Hardening (S3)** — ioctl fuzzer, KASAN/lockdep kernel, repeated load/unload stress.

## testrk fixtures (Session 2, VM-only, benign)
| Fixture | Technique | Expected finding |
|---|---|---|
| `testrk_hidemod` | unlink self from module list | `hidden-module.unlinked` |
| `testrk_sysfs` | remove sysfs kobject | `hidden-module.sysfs-only` |
| `testrk_syscall` | swap a harmless syscall pointer to a module function that calls the original | `syscall-table.in-module` |
| `testrk_hidepid` | hide a chosen pid from `/proc` | `hidden-process.not-in-proc` |
Each fixture logs loudly to `dmesg`, has a `MODULE_DESCRIPTION("REDOUBT TEST FIXTURE - benign")`, restores state on unload.

## Evaluation (Session 4)
Table of detector × fixture (detected/missed/FP), scan latency, kmod overhead — feeds the course report.
