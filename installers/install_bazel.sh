#!/usr/bin/env bash

set -e

BAZEL_VERSION=0.20.0
PLATFORM=`uname -s | tr '[:upper:]' '[:lower:]'`
FILE="bazel-${BAZEL_VERSION}-installer-${PLATFORM}-x86_64.sh"

wget https://github.com/bazelbuild/bazel/releases/download/${BAZEL_VERSION}/${FILE}
chmod +x ${FILE}
./${FILE} --user