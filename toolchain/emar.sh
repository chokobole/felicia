#!/bin/bash

# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -euo pipefail

source bazel-out/host/genfiles/toolchain/env.sh

$PYTHON2_BIN external/emscripten_toolchain/emar.py "$@"
