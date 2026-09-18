# Threat Model

## In scope
Loadable-kernel-module (LKM) rootkits on Linux (target kernels 6.1–6.8, x86_64/aarch64) running as
root-equivalent kernel code that try to hide themselves or attacker activity.

## Technique → detector map
| Technique | Example | Redoubt detector |
|---|---|---|
| Unlink module from `modules` list | `list_del(&THIS_MODULE->list)` | `hidden-module` (kobject / `/sys/module` / mod_tree views vs `/proc/modules`) |
| Remove sysfs kobject | `kobject_del(&THIS_MODULE->mkobj.kobj)` | `hidden-module` (kernel list vs sysfs) |
| Syscall table hooking | overwrite `sys_call_table[__NR_getdents64]` | `syscall-table` |
| ftrace/kprobe hooking | `ftrace_set_filter_ip` on syscall wrappers | `ftrace-hooks` |
| Hide processes | filter `/proc` readdir, unlink task from lists | `hidden-process` |
| Hide files/dirs | hook `getdents64`/`filldir` | `hidden-file` |
| Netfilter hooks / port hiding | `nf_register_net_hook` | `net-hooks` |
| Unsigned/out-of-tree module load | `insmod` without signature | `taint` |

## Trust assumptions
- The scanner runs as root; the kmod is loaded from a trusted build **before** compromise when possible.
- Kernel text/`rodata` and Redoubt's own code are assumed intact at scan time.

## Out of scope / known limits (be honest in reports)
- **A rootkit with equal kernel privilege can tamper with Redoubt itself** (hook our ioctl, patch our
  module, alter the kernel structures we read). This is an arms race; Redoubt raises the bar, it does not end it.
- Rootkits that load *before* Redoubt can hide from all in-kernel views. Mitigation: load kmod early / at boot; out-of-band checks (memory forensics) are out of scope.
- Hypervisor/firmware/bootkit rootkits, eBPF-only rootkits (future work), userland LD_PRELOAD rootkits.
- False positives: legitimate tools using ftrace/kprobes (eBPF tracers, livepatch, security agents) — handled by allowlist (Session 3).

## Safety of Redoubt itself
Read-only kmod, root-only device, bounded copies, no writes to kernel state; fuzzed in Session 3.
