#!/usr/bin/env bash
# Smoke test in the VM: load kmod, check `redoubt doctor`, unload, check dmesg.
# Session 2+ extends this with testrk integration tests (tests/integration/).
set -euo pipefail
VM=redoubt
"$(dirname "$0")/vm-sync-build.sh"
# shellcheck disable=SC2016  # expansion intentionally happens inside the VM
limactl shell "$VM" -- sudo bash -euc '
	cd "$HOME/redoubt-build" 2>/dev/null || cd /home/*/redoubt-build
	insmod kmod/redoubt_kmod.ko
	trap "rmmod redoubt_kmod" EXIT
	./build/redoubt doctor | tee /dev/stderr | grep -q "kmod     : ok"
	./build/redoubt scan --json >/dev/null
	echo "smoke test OK"
'
