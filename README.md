# Redoubt

**Redoubt** is an open-source detector for Linux kernel-module rootkits. It uses *cross-view
detection*: the same fact (loaded modules, syscall targets, tasks) is collected from independent
sources, and disagreements are reported as findings.

```
            ┌────────────── redoubt CLI (C11) ──────────────┐
            │  detectors: hidden-module · syscall-table ...  │
            │            compare views → findings            │
            └───────┬───────────────┬──────────────┬─────────┘
                    │               │              │
              /proc source    /sys source     kmod source
                                                   │ ioctl
                                          /dev/redoubt (root)
                                          redoubt_kmod.ko (read-only)
```

> **Status:** early development (Session 1 of 4: scaffold + docs). No detectors are implemented yet.
> Redoubt is a **detection** tool. It cannot defeat a rootkit with equal kernel privilege; see
> [docs/THREAT_MODEL.md](docs/THREAT_MODEL.md).

## Quickstart
```bash
make scanner test          # portable build + unit tests
build/redoubt list-checks
build/redoubt doctor
```
Kernel module (Linux only; use the VM on macOS, see [docs/DEV_ENVIRONMENT.md](docs/DEV_ENVIRONMENT.md)):
```bash
tools/vm-up.sh && tools/vm-test.sh
```

## Docs
[Architecture](docs/ARCHITECTURE.md) · [Threat model](docs/THREAT_MODEL.md) ·
[Detectors](docs/DETECTORS.md) · [Kmod ABI](docs/KMOD_ABI.md) · [Kernel compat](docs/KERNEL_COMPAT.md) ·
[Dev environment](docs/DEV_ENVIRONMENT.md) · [Testing](docs/TESTING.md) · [Roadmap](docs/ROADMAP.md) ·
[Contributing](docs/CONTRIBUTING.md) · [Security](docs/SECURITY.md)

## License
GPL-2.0-only. See [LICENSE](LICENSE).
