#!/bin/bash

# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set -euo pipefail

argv=("$@")
tarfile=
# Find the -o option, and strip the .tar from it.
for (( i=0; i<$#; i++ )); do
  if [[ "x${argv[i]}" == x-o ]]; then
    arg=${argv[$((i+1))]}
    if [[ "x$arg" == x*.tar ]];then
        tarfile="$(basename "$arg")"
        emfile="$(dirname "$arg")/$(basename $arg .tar)"
        basearg="$(basename "$(basename "$(basename "$emfile" .js)" .html)" .wasm)"
        baseout="$(dirname "$arg")/$basearg"
        argv[$((i+1))]="$emfile"
    fi
  fi
done

# Run emscripten to compile and link
emcc "${argv[@]}"

# Remove the first line of .d file
find . -name "*.d" -exec sed -i.bak -e '1d' {} \;
find . -name "*.d.bak" -exec rm {} \;

# Now create the tarfile
shopt -s extglob
if [ "x$tarfile" != x ]; then
  outdir="$(dirname "$baseout")"
  outbase="$(basename "$baseout")"
  (
      cd "$outdir";
      tar cf "$tarfile" "$outbase."?(html|js|wasm|mem|data|worker.js)
  )
fi