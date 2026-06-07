#!/usr/bin/env bash
# Build only type_traist_notebook + py_visualizer into infrastructure/.inf_install.
# Avoids full build.py (MKL/CUDA). Used by optics_toolkits CI.
set -euo pipefail

INFRA_DIR="${1:?usage: ci_infra_minimal.sh /path/to/infrastructure}"
PREFIX="${INFRA_DIR}/.inf_install"
CXX="${CXX:-clang++-20}"
CC="${CC:-clang-20}"

mkdir -p "${PREFIX}"

build_and_install() {
    local src="$1"
    local build="${src}/build"
    cmake -S "${src}" -B "${build}" \
        -DCMAKE_CXX_COMPILER="${CXX}" \
        -DCMAKE_C_COMPILER="${CC}" \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="${PREFIX}" \
        -DCMAKE_PREFIX_PATH="${PREFIX}"
    cmake --build "${build}" -j"$(nproc)"
    cmake --install "${build}"
}

echo "==> type_traist_notebook -> ${PREFIX}"
build_and_install "${INFRA_DIR}/type_traist_notebook"

REQ="${INFRA_DIR}/py_visualizer/requirements.txt"
if [ -f "${REQ}" ]; then
    echo "==> pip install ${REQ}"
    python3 -m pip install --user -r "${REQ}"
fi

echo "==> py_visualizer -> ${PREFIX}"
# build.py sets CPLUS_INCLUDE_PATH so py_visualizer finds type_traist_notebook headers
# (find_package only provides the config file, not -I for header-only installs).
export CPLUS_INCLUDE_PATH="${PREFIX}/include${CPLUS_INCLUDE_PATH:+:${CPLUS_INCLUDE_PATH}}"
build_and_install "${INFRA_DIR}/py_visualizer"

echo "CMAKE_PREFIX_PATH=${PREFIX}"
