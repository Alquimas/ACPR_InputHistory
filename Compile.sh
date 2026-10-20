#!/usr/bin/env bash
set -euo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_DIR/build"
MOD_OUTPUT_DIR="$PROJECT_DIR/bin/ACPR_InputHistory"

if [[ ! -f "$BUILD_DIR/CMakeCache.txt" ]]; then
    echo "Running CMake configuration..."

    if [[ "${OSTYPE:-}" == msys* || "${OSTYPE:-}" == cygwin* ||
          "${OSTYPE:-}" == win32* ]]; then
        cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" -G "MinGW Makefiles"
    else
        cmake -S "$PROJECT_DIR" -B "$BUILD_DIR" \
            -DCMAKE_TOOLCHAIN_FILE="$PROJECT_DIR/toolchain.cmake"
    fi
else
    echo "Build directory already configured."
fi

cmake --build "$BUILD_DIR"

if [[ -n "${GGXXACPR_MOD_DIR:-}" ]]; then
    if [[ ! -d "$GGXXACPR_MOD_DIR" ]]; then
        printf 'GGXXACPR_MOD_DIR is not a directory: %s\n' \
            "$GGXXACPR_MOD_DIR" >&2
        exit 1
    fi

    echo
    echo "Copying files to mod folder:"
    cp -ruv -- "$MOD_OUTPUT_DIR" "$GGXXACPR_MOD_DIR/"
fi
