#!/usr/bin/env bash
# Verify MATLAB bindings layout and expected configure skip when MATLAB is absent.
set -euo pipefail

ROOT="$(cd "$(dirname "${0}")/.." && pwd)"
BUILD_DIR="${1:-}"
CMAKE_LOG="${2:-}"

MATLAB_DIR="${ROOT}/bindings/matlab"
OPTICS_M="${MATLAB_DIR}/+optics"

required=(
    "${MATLAB_DIR}/optics_tmm_mex.c"
    "${MATLAB_DIR}/CMakeLists.txt"
    "${OPTICS_M}/tmm.m"
    "${OPTICS_M}/write_csv.m"
)

for f in "${required[@]}"; do
    if [ ! -f "${f}" ]; then
        echo "missing required file: ${f}" >&2
        exit 1
    fi
done

count="$(find "${OPTICS_M}" -maxdepth 1 -name '*.m' | wc -l)"
if [ "${count}" -lt 2 ]; then
    echo "expected at least 2 .m files in +optics, found ${count}" >&2
    exit 1
fi

if [ -n "${CMAKE_LOG}" ] && [ -f "${CMAKE_LOG}" ]; then
    if ! grep -q "MATLAB not found; skipping" "${CMAKE_LOG}"; then
        echo "configure log missing expected skip message" >&2
        exit 1
    fi
fi

if [ -n "${BUILD_DIR}" ] && [ -d "${BUILD_DIR}" ]; then
    if find "${BUILD_DIR}" -name 'optics_tmm_mex*' -print -quit | grep -q .; then
        echo "unexpected MEX artifact under ${BUILD_DIR}" >&2
        exit 1
    fi
fi

echo "matlab bindings verification passed"
