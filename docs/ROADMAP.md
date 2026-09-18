# Roadmap (4 Claude Code sessions)

Each session ends with: `make scanner test` green, docs updated, CLAUDE.md status ticked, commit.

## Session 1 — Foundation ✅
- [x] Repo scaffold, license notice, editor/format config, CI
- [x] Shared ABI header; kmod registers `/dev/redoubt`, answers `GET_VERSION`
- [x] Scanner: registry, findings, text/JSON reporters, sources (proc parse), CLI (`scan`, `list-checks`, `doctor`, `version`)
- [x] Unit test harness (65 checks), Lima VM tooling
- [x] Docs: architecture, threat model, detectors, ABI, compat, dev env, testing, contributing, security
- [ ] (needs VM, verify first in Session 2) `tools/vm-test.sh` end-to-end; full GPL-2.0 text in LICENSE

## Session 2 — Core detectors
- [ ] Verify Session 1 VM smoke test; fix any kmod build issues on 6.8
- [ ] kmod: symbol resolution helper (kprobe trick), `LIST_MODULES`, `GET_TEXT_RANGE`, `READ_SYSCALL_TABLE`, `LIST_TASKS` (+ ABI/docs)
- [ ] Sources: `sysfs` list_modules, `kmod` ops, `proc` tasks
- [ ] Detectors: `hidden-module`, `syscall-table`, `hidden-process`, `taint` in `scanner/detectors/`
- [ ] `testrk/` fixtures (hidemod, sysfs, syscall, hidepid) + `tests/integration/*.sh`
- [ ] Unit tests with mock sources for every detector; clean-VM zero-findings test

## Session 3 — Advanced detectors + hardening
- [ ] `ftrace-hooks`, `hidden-file`, `net-hooks` (+ ioctls `LIST_FTRACE`, `LIST_NF_HOOKS`)
- [ ] Allowlist file (`/etc/redoubt/allow.conf`, `key = value` lines) + `--allowlist` flag; severity tuning
- [ ] kmod hardening: locking review, size caps, `-ENOSPC` protocol everywhere
- [ ] Debug kernel (KASAN/lockdep) run; ioctl fuzzer in `tests/integration/fuzz_ioctl.c`; load/unload stress

## Session 4 — Polish and release
- [ ] `redoubt monitor` (periodic rescan, JSON events, syslog)
- [ ] DKMS packaging, man page (`docs/redoubt.1`), `.deb` build script
- [ ] Evaluation table + benchmarks; demo script; report skeleton for the course
- [ ] Final docs pass, full LICENSE text, SPDX headers audit, issue templates, `v0.1.0` tag
