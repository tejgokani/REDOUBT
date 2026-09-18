# Architecture

## Principle: cross-view detection
A rootkit lies through one interface (e.g. removes itself from `/proc/modules`) but rarely through
all of them consistently. Redoubt gathers the same fact from N independent views and reports any
disagreement. The kernel module adds views that don't go through the syscall/VFS paths rootkits
usually hook.

## Components
| Component | Path | Responsibility |
|---|---|---|
| Shared ABI | `include/redoubt_abi.h` | ioctl numbers and structs (see KMOD_ABI.md) |
| Kernel module | `kmod/` | Read-only introspection via `/dev/redoubt` (0600, `CAP_SYS_ADMIN`) |
| Sources | `scanner/source_*.c` | One view each: `proc`, `sysfs`, `kmod` (+ mocks in tests) |
| Detectors | `scanner/detector.c`, later `scanner/detectors/*.c` | Compare views, emit findings |
| Findings | `scanner/finding.c` | `{id, detector, severity, message, evidence[]}` |
| Reporters | `scanner/report.c` | Text and JSON (`"schema":1`) |
| CLI | `scanner/main.c` | `scan`, `list-checks`, `doctor`, `version` |

## Data flow
`main` builds a `scan_ctx` (pointers to sources) → for each selected detector: skip as
`unavailable` if it `needs_kmod` and the kmod probe fails, else call `run(ctx, &findings)` →
`report_text` / `report_json`.

## Interfaces
- `struct source { name, probe, list_modules, ... priv }` — ops return `0` or `-errno`; `-ENOSYS` = view unsupported. New ops are added per detector (Session 2: `list_tasks`, `syscall_table`, `text_range`).
- `struct detector { name, description, needs_kmod, run }` returning `enum detect_status` (`ok`, `not-implemented`, `unavailable`, `error`).
- Exit codes: `0` clean, `1` findings, `2` error/usage. `unavailable` alone is not an error (reduced coverage is warned about).

## Graceful degradation
No kmod → detectors that need it are `unavailable`; the rest still run. `doctor` explains what's missing.

## Concurrency / safety
Scanner is single-threaded. The kmod holds no long-lived locks in ioctl paths; module-list walks take
the required locks (`module_mutex` / RCU) and copy to a bounded kernel buffer before `copy_to_user`.

## Platform
Scanner core builds on macOS for unit tests; Linux-only code sits behind `#ifdef __linux__` with `-ENOSYS` stubs.
