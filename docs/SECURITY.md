# Security policy

Redoubt includes kernel code running with full privilege, so bugs are security-relevant.

## Reporting
Please report vulnerabilities privately (GitHub Security Advisories on the repo once published; until then contact the maintainers directly) — not in public issues. Include kernel version, steps, and `dmesg` output. We aim to acknowledge within 7 days.

## Scope
In scope: the kmod (`kmod/`), its ioctl interface, the scanner's parsing of untrusted data. Out of scope: rootkits evading detection (see `THREAT_MODEL.md`; report those as normal issues).

## Design commitments
Root-only device, `CAP_SYS_ADMIN` on every ioctl, read-only behaviour, bounded copies, fuzzing before release.

## Supported versions
Only the latest release until 1.0.
