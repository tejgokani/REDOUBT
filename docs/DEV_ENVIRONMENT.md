# Development environment

The Mac cannot build or load Linux kernel modules. Use an aarch64 Ubuntu 24.04 VM via [Lima](https://lima-vm.io).

## One-time setup
```bash
brew install lima        # also installs QEMU dependency handling
tools/vm-up.sh           # creates VM "redoubt", mounts repo at /redoubt, installs headers
limactl snapshot create redoubt --tag clean
```

## Daily loop
```bash
make scanner test        # on the Mac (fast, portable)
tools/vm-test.sh         # builds in VM, insmod, smoke test, rmmod
limactl shell redoubt    # interactive; use `dmesg -w` in a second shell
```
The VM builds from an rsync'd copy (`~/redoubt-build`) so macOS object files never leak in.

## Safe-load procedure (any kmod / testrk change)
1. `limactl snapshot create redoubt --tag pre-load`
2. `sudo insmod ...`, run tests, `sudo rmmod ...`, check `dmesg | tail`.
3. If the VM panics/hangs: `limactl stop -f redoubt && limactl snapshot apply redoubt --tag pre-load`.
- **Never** load `testrk` outside this VM.

## Debug kernel (Session 3)
Second VM/kernel with `CONFIG_KASAN`, `CONFIG_PROVE_LOCKING`, `CONFIG_DEBUG_ATOMIC_SLEEP` for kmod hardening.

## Troubleshooting
- `make kmod` "No such file … build": `sudo apt install linux-headers-$(uname -r)` in VM.
- Module load rejected: check `dmesg` (signature enforcement, see KERNEL_COMPAT.md).
