# Kernel module ABI (`include/redoubt_abi.h`)

## Device
`/dev/redoubt`, misc device, mode `0600`, every ioctl requires `CAP_SYS_ADMIN`. Read-only: no ioctl may modify kernel state.

## Versioning
`RDT_ABI_MAJOR.RDT_ABI_MINOR`. Scanner refuses (`doctor` shows INCOMPATIBLE) if the major differs; a lower module minor just means fewer ioctls (`-ENOTTY`).

## Conventions
- Fixed-width types only (`rdt_u32/u64`); no pointers except `rdt_u64` user addresses; explicit padding.
- Variable-size results: request struct `{ u64 buf; u32 buf_len; u32 count; u32 needed; u32 flags }`. Kernel writes at most `buf_len` bytes, sets `needed` to the full size and returns `-ENOSPC` when too small; caller retries with a bigger buffer. Kernel takes a snapshot under lock into a bounded kmalloc buffer (hard cap 4 MiB) then `copy_to_user` outside the lock.
- Errors: `-EPERM` (no cap), `-EFAULT`, `-EINVAL` (nonzero reserved), `-ENOSPC`, `-ENOTTY` (unknown), `-EOPNOTSUPP` (kernel cannot provide it, e.g. symbol unresolvable).

## ioctls (magic `'R'`)
| nr | Name | Status | Result |
|---|---|---|---|
| 0x01 | `RDT_GET_VERSION` | **Session 1** | `struct rdt_version { abi_major, abi_minor, kernel_version_code, flags }` |
| 0x02 | `RDT_LIST_MODULES` | S2 | records `{name[56], base, size, state, taints}` from the kernel module list |
| 0x03 | `RDT_GET_TEXT_RANGE` | S2 | `{stext, etext, sinittext, einittext}` |
| 0x04 | `RDT_READ_SYSCALL_TABLE` | S2 | array of `u64` entry addresses + `nr_syscalls` |
| 0x05 | `RDT_LIST_TASKS` | S2 | `{pid, tgid, comm[16]}` from `for_each_process` |
| 0x06 | `RDT_LIST_FTRACE` | S3 | `{callback addr, owning-module name}` |
| 0x07 | `RDT_LIST_NF_HOOKS` | S3 | `{hook addr, pf, hooknum, priority}` |

Each new ioctl: add struct to the header, document here, add a userspace source op, add a fuzz case (S3).
