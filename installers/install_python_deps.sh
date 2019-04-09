#!/usr/bin/env bash

set -e

curl -O https://bootstrap.pypa.io/get-pip.py

if [ -v PYTHON_BIN_PATH ]; then
  PYTHON_BIN="$PYTHON_BIN_PATH"
else
  PYTHON_BIN="python"
fi
"$PYTHON_BIN" --version
"$PYTHON_BIN" get-pip.py --user

pip install numpy --user