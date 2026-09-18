#!/usr/bin/env bash
# Build scanner + kmod inside the VM (repo is mounted at /redoubt).
# Build in a VM-local copy so host build artifacts (macOS objects) never mix in.
set -euo pipefail
VM=redoubt
limactl shell "$VM" -- bash -euc '
	rsync -a --delete --exclude build --exclude .git /redoubt/ "$HOME/redoubt-build/"
	cd "$HOME/redoubt-build"
	make clean >/dev/null 2>&1 || true
	make scanner test kmod
'
