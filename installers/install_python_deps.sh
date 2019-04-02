#!/usr/bin/env bash

set -e

curl -O https://bootstrap.pypa.io/get-pip.py

if [ "$1" == "--three" ]; then
  PYTHON_BIN="python3"
else
  PYTHON_BIN="python"
fi
"$PYTHON_BIN" --version
"$PYTHON_BIN" get-pip.py --user

pip install numpy --user