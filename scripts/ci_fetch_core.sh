#!/usr/bin/env bash
# Shallow-clone pinned simulation into core/ for CI (bypasses git submodule).
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
REV="63e0f06fe9047cfafe9a687c4677605f648d9ecb"
REPO="${SIMULATION_REPO:-git@github.com:likooooo/simulation.git}"

rm -rf "${ROOT}/core"
git clone --depth 1 "${REPO}" "${ROOT}/core"
git -C "${ROOT}/core" fetch --depth 1 origin "${REV}"
git -C "${ROOT}/core" checkout "${REV}"
echo "core at $(git -C "${ROOT}/core" rev-parse HEAD)"
