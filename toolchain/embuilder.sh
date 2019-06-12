#!/bin/bash
set -euo pipefail

source bazel-out/host/genfiles/toolchain/env.sh

mkdir -p "$PWD/toolchain/emscripten_cache"

export EM_CACHE="$PWD/toolchain/emscripten_cache"

$PYTHON2_BIN bazel-felicia/external/emscripten_toolchain/embuilder.py "$@"