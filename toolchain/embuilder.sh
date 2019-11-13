#!/bin/bash

# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -euo pipefail

source bazel-out/host/genfiles/toolchain/env.sh

mkdir -p "$PWD/toolchain/emscripten_cache"

export EM_CACHE="$PWD/toolchain/emscripten_cache"

$PYTHON2_BIN bazel-felicia/external/emscripten_toolchain/embuilder.py "$@"