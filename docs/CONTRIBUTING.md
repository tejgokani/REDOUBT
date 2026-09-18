# Contributing to Redoubt

1. Read `docs/ARCHITECTURE.md` and `CLAUDE.md` (conventions apply to humans too).
2. `make scanner test` must pass with `-Werror`. Kmod changes must build on kernel 6.8 and pass `tools/vm-test.sh`.
3. New detector: register in `scanner/detector.c`, spec in `docs/DETECTORS.md`, unit test with mock sources.
4. ABI change: update header and `docs/KMOD_ABI.md` together, bump version.
5. Style: Linux kernel style (`.clang-format`), tabs, SPDX header on every file.
6. Detection only: patches adding evasion or offensive capability outside `testrk/` will be rejected.
7. Small commits, imperative messages; sign off your work.

Be respectful; see `CODE_OF_CONDUCT.md`.
