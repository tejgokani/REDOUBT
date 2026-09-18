#!/usr/bin/env bash
# Create/start the Redoubt dev VM. Requires: brew install lima
set -euo pipefail
REPO_DIR="$(cd "$(dirname "$0")/.." && pwd)"
VM=redoubt

command -v limactl >/dev/null || { echo "install lima first: brew install lima" >&2; exit 1; }

if limactl list -q | grep -qx "$VM"; then
	limactl start "$VM"
else
	tmp="$(mktemp)"
	sed "s|REPO_DIR|$REPO_DIR|" "$REPO_DIR/tools/lima/redoubt.yaml" >"$tmp"
	limactl start --name="$VM" --tty=false "$tmp"
	rm -f "$tmp"
fi
echo "VM ready. Snapshot before loading modules: limactl snapshot create $VM --tag clean"
