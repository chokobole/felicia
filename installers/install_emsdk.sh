#!/usr/bin/env bash

set -e

COMMIT=c10e3e86e8ead9243473c7148beaa9261956123b
TARGET=1.39.0

cd toolchain/emscripten
git clone https://github.com/emscripten-core/emsdk.git

cd emsdk
git checkout $COMMIT

UNAME=`uname`
if [[ $UNAME == CYGWIN_NT* ]] || [[ $UNAME == MINGW64_NT* ]]; then
  ./emsdk.bat install $TARGET
  ./emsdk.bat activate $TARGET
else
  ./emsdk install $TARGET
  ./emsdk activate $TARGET
  source ./emsdk_env.sh

  embuilder.py build libcompiler_rt libc libc_rt_wasm libc-wasm libc++ libc++-noexcept libc++abi libc++abi-noexcept libdlmalloc libembind-rtti libpthread_stub struct_info libjpeg
fi
