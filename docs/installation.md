# Installation

## Contents
- [Installation](#installation)
  - [Contents](#contents)
  - [Prerequisites](#prerequisites)
    - [Ubuntu or Mac](#ubuntu-or-mac)
    - [Windows](#windows)
  - [How to build](#how-to-build)
  - [How to package](#how-to-package)
    - [Common](#common)
    - [Ubuntu](#ubuntu)
    - [Windows](#windows-1)
    - [Mac](#mac)
  - [How to bind different language](#how-to-bind-different-language)
    - [python](#python)
      - [Windows](#windows-2)
      - [Elsewhere](#elsewhere)
    - [wasm](#wasm)
    - [node](#node)
      - [Windows](#windows-3)
      - [Elsewhere](#elsewhere-1)

## Prerequisites

We use [bazel](https://www.bazel.build/) as a build tool. To build project, you need to down load and install it! Currently we tested with version `0.25.0`. Also we need some other dependencies to bind different languages, and we provided you scripts for it.


### Ubuntu or Mac

```bash
./installers/install_bazel.sh
./installers/install_python_deps.sh # Maybe --user flags needed
./installers/install_nodejs.sh
```

### Windows

You have to download llvm from [llvm.org](http://llvm.org/builds/) also. Because we need `clang-cl` to build. For `bazel` to make use of it, you have to set `USE_CLANG_CL` to `1` and `BAZEL_LLVM` to `/path/to/llvm` if you didn't install llvm at default location.

```bash
choco install bazel --version 0.25.0
./installers/install_python_deps.sh # Maybe --user flags needed
choco install nodejs.install --version 10.15.3
```

## How to build

```bash
bazel build //felicia/...
```

To communication ROS1 topics or services, you have to pass `--define has_ros=true` like below.

```bash
bazel build --define has_ros=true //felicia/...
```

For the developers working on **mac**, please note that currently, you have to manually set `darwin_x86_64` on every command or we recommend to use `.bazelrc.user` like below.

```bash
echo 'build --cpu darwin_x86_64' >> .bazelrc.user
```

## How to package

### Common

To generate a tar file containing headers, run the command like below. The output will be generated at `bazel-bin/felicia/felicia_hdrs.tar`.

```bash
bazel build --define framework_shared_object=true //felicia:felicia_hdrs
```

### Ubuntu

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/libfelicia.so`.

```bash
bazel build --define framework_shared_object=true //felicia:libfelicia.so
```

### Windows

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/felicia.dll`. The interface library will be generated at `bazel-bin/felicia/felicia.lib`.

```bash
bazel build --define framework_shared_object=true //felicia:felicia.dll
bazel build --define framework_shared_object=true //felicia:felicia_lib
```

### Mac

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/libfelicia.dylib`.

```bash
bazel build --cpu darwin_x86_64 --define framework_shared_object=true //felicia:libfelicia.dylib
```

## How to bind different language

### python

#### Windows

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/python/felicia_py.pyd`.

```bash
bazel build --define framework_shared_object=true //felicia/python:felicia_py_pyd
```

#### Elsewhere

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/python/felicia_py.so`.

```bash
bazel build --define framework_shared_object=true //felicia/python:felicia_py.so
```

### wasm

**On windows it's not supported yet.**

```bash
bazel build --config wasm //felicia/wasm:felicia_wasm
```

Once you failed with the reason below,

```bash
Use --sandbox_debug to see verbose messages from the sandbox
cp: cannot stat 'toolchain/emscripten_cache/*': No such file or directory
```

Then you should run command below. Please refer to section 9 in [bazel tutorial](https://docs.bazel.build/versions/0.25.0/tutorial/cc-toolchain-config.html#configuring-the-c-toolchain).

```bash
./toolchain/embuilder.sh build compiler-rt dlmalloc libc libc-wasm libc++ libc++_noexcept libc++abi pthreads
```

And if you try again, you will find that it succeds.

```bash
bazel build --config wasm //felicia/wasm:felicia_wasm
```

### node

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/js/felicia_js.node`.

#### Windows

On windows, when linking against `node.lib`, symbols are conflited between `node.lib`(`zlib` and `openssl`) and `grpc`(`zlib` and `boringssl`). So you should build a `felicia_js.node` without `grpc` library. You can do like below.

**Please try not with `-c dbg`, because `node.lib` is provided with only release version.**

```bash
bazel build --config node_binding //felicia/js:felicia_js_node
```

#### Elsewhere

```bash
bazel build --config node_binding //felicia/js:felicia_js_node
```