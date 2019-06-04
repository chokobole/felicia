#!/bin/bash
set -euo pipefail

source bazel-out/host/bin/toolchain/env.sh

mkdir -p "tmp/emscripten_cache"

# Prepare the cache content so emscripten doesn't keep rebuilding it
cp -r toolchain/emscripten_cache/* tmp/emscripten_cache

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
$PYTHON2_BIN external/emscripten_toolchain/emcc.py "${argv[@]}"

# Remove the first line of .d file
find . -name "*.d" -exec sed -i.bak '2d' {} \;
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
