# Redoubt — Claude Code project guide

Redoubt is an open-source, **defensive** Linux kernel-module rootkit detector (OS course project).
Read `docs/ARCHITECTURE.md` and `docs/ROADMAP.md` before starting any session.

## Architecture in one paragraph
Cross-view detection. A read-only kernel module (`kmod/`, exposes `/dev/redoubt` + ioctls) provides
ground-truth views. A C11 userspace CLI (`scanner/`) gathers views from independent **sources**
(`proc`, `sysfs`, `kmod`, test mocks), and **detectors** flag disagreements as **findings**.
Detectors never read the system directly; they only use `struct source` so they are unit-testable.

## Layout
- `include/redoubt_abi.h` — ioctl ABI shared by kmod and scanner (spec: `docs/KMOD_ABI.md`)
- `scanner/` — CLI: `main.c`, `detector.c` (registry), `finding.c`, `report.c`, `source_*.c`, `util.c`
- `kmod/` — kernel module (GPL-2.0-only, Kbuild)
- `testrk/` — benign test rootkit fixtures (Session 2+; VM-only, never installed by default)
- `tests/unit/` — host-runnable unit tests; `tests/integration/` — run in the VM
- `tools/` — Lima VM scripts. `docs/` — all specs.

## Commands
- `make scanner` / `make test` — build + unit tests (works on macOS and Linux)
- `make kmod` — Linux only. On the Mac use `tools/vm-sync-build.sh`
- `tools/vm-up.sh` (create VM), `tools/vm-test.sh` (build, insmod, smoke test, rmmod)

## Rules
1. **Detection only.** Never add code that hides, hooks, or modifies kernel state outside `testrk/`.
   `testrk/` is clearly labelled, benign, and VM-only. The main kmod is strictly read-only.
2. **Never load any module on the host Mac** (impossible anyway) or suggest loading on a real machine.
   Take a VM snapshot (`limactl snapshot create redoubt --tag clean`) before loading new kmod code.
3. Kernel code: Linux kernel coding style (tabs, 80–100 cols), every `copy_to_user` size-checked,
   ioctl requires `CAP_SYS_ADMIN`, no unbounded allocations from user-controlled sizes.
4. Userspace: C11, must pass `-Wall -Wextra -Wpedantic -Werror`. No external deps. Check every
   allocation. Linux-only code goes behind `#ifdef __linux__` with a `-ENOSYS` stub so host tests build.
5. ABI changes: update `include/redoubt_abi.h` AND `docs/KMOD_ABI.md` together; bump `RDT_ABI_MINOR`
   (additive) or `RDT_ABI_MAJOR` (breaking).
6. Adding a detector: register it in `scanner/detector.c`, spec it in `docs/DETECTORS.md`, add a unit
   test with mock sources, and an integration test if it needs the kmod. Keep detector names
   consistent across code, docs, and `tests/unit/test_detector.c`.
7. Findings have stable ids (`<detector>.<reason>`); never rename released ids.
8. Exit codes: 0 clean, 1 findings, 2 error. JSON output has `"schema":1`; additive changes only.
9. Honest about limits: document evasion possibilities in `docs/THREAT_MODEL.md`; don't overclaim.
10. Commit messages: imperative, small commits per logical change.

## Definition of done (every session)
`make scanner test` green, `-Werror` clean, docs updated for anything changed, ROADMAP checkboxes ticked,
and (for kmod changes) `tools/vm-test.sh` passes with clean `dmesg`.

## Status (update at the end of each session)
- [x] Session 1 — foundation, docs, scaffold (kmod GET_VERSION, CLI: version/list-checks/doctor/scan stubs)
- [ ] Session 2 — core detectors (hidden-module, syscall-table, taint, hidden-process) + testrk
- [ ] Session 3 — advanced detectors, allowlist, hardening/fuzzing
- [ ] Session 4 — monitor mode, packaging, evaluation, v0.1.0 release
