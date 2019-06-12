#!/bin/bash
set -euo pipefail

source bazel-out/host/genfiles/toolchain/env.sh

$PYTHON2_BIN external/emscripten_toolchain/emar.py "$@"
