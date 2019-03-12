#!/usr/bin/env bash

set -e

curl -O https://bootstrap.pypa.io/get-pip.py

python3 get-pip.py --user

pip install numpy --user