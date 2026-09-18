# Kernel compatibility

## Targets
| Kernel | Arch | Status |
|---|---|---|
| 6.8 (Ubuntu 24.04) | aarch64 | **primary dev/test** (Lima VM) |
| 6.8 | x86_64 | CI build-only |
| 6.6 / 6.1 LTS | x86_64/aarch64 | best effort, via `LINUX_VERSION_CODE` shims |

## Known API drift to handle (Sessions 2–3)
- `kallsyms_lookup_name` unexported since 5.7: resolve via a temporary `kprobe` on the symbol and read `kp.addr`, then `unregister_kprobe`. Needs `CONFIG_KPROBES`.
- `sys_call_table` not exported; on newer kernels/arm64 it may be `const`/read-only — we only **read** it.
- Module internals (`struct module`, `mod_tree`, `module_mutex`) change between releases (6.x moved parts to `kernel/module/`); wrap each in one accessor with `#if LINUX_VERSION_CODE` branches.
- `CONFIG_MODULE_SIG_FORCE` / lockdep / Secure Boot can prevent loading an unsigned Redoubt module — `doctor` must explain (sign the module or use a dev VM).
- Symbols may be unresolvable on hardened kernels: return `-EOPNOTSUPP`, scanner marks the detector `unavailable`.

## Policy
Every kmod change must build against 6.8 in CI; document any shim here.
