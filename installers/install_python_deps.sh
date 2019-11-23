#!/usr/bin/env bash

set -e

if ! [ -x "$(command -v pip3)" ]; then
  curl -O https://bootstrap.pypa.io/get-pip.py

  if [ -z "$PYTHON_BIN_PATH" ]; then
    PYTHON_BIN="python"
  else
    PYTHON_BIN="$PYTHON_BIN_PATH"
  fi

  "$PYTHON_BIN" get-pip.py --user
fi

pip3 install numpy $1
# Disable temporarily due to issue https://github.com/grpc/grpc/issues/20831
# pip3 install grpcio $1
pip3 install empy $1  # For ROS C++ message generation
pip3 install pyyaml $1  # For ROS Python message generation