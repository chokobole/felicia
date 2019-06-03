BAZEL_FELICIA=%{BAZEL_FELICIA}
PYTHON2_BIN=%{PYTHON2_BIN}

export EMSCRIPTEN="$BAZEL_FELICIA/external/emscripten_toolchain"
export LLVM="$BAZEL_FELICIA/external/emscripten_clang"
export BINARYEN="$BAZEL_FELICIA/external/emscripten_clang/binaryen"

export EMSCRIPTEN_NATIVE_OPTIMIZER="$BAZEL_FELICIA/external/emscripten_clang/optimizer"

export SPIDERMONKEY=''
export V8=''

export EM_EXCLUSIVE_CACHE_ACCESS=1
export EMCC_SKIP_SANITY_CHECK=1
export EMCC_WASM_BACKEND=0
# export EMCC_DEBUG=1

export EM_CACHE="tmp/emscripten_cache"
export TEMP_DIR="tmp"