#!/bin/bash
set -euo pipefail

source bazel-out/host/bin/toolchain/env.sh

$PYTHON2_BIN external/emscripten_toolchain/emar.py "$@"
