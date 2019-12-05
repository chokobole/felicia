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
  - [How to communication ROS1 topic / service](#how-to-communication-ros1-topic--service)

## Prerequisites

We use [bazel](https://www.bazel.build/) as a build tool. To build project, you need to down load and install it! Currently we tested with version `0.25.0`. Also we need some other dependencies to bind different languages, and we provided you scripts for it.


### Ubuntu or Mac

```bash
./installers/install_bazel.sh
```

For the developers working on **mac**, please note that currently, you have to manually set `darwin_x86_64` on every command or we recommend to use `.bazelrc.user` like below.

```bash
echo 'build --cpu darwin_x86_64' >> .bazelrc.user
```

### Windows

You have to download llvm from [llvm.org](http://llvm.org/builds/) also. Because we need `clang-cl` to build. For `bazel` to make use of it, you have to set `USE_CLANG_CL` to `1` and `BAZEL_LLVM` to `/path/to/llvm` if you didn't install llvm at default location.

```bash
choco install bazel --version 0.25.0
```

## How to build

```bash
bazel build //felicia/...
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

We need some python packages, please run the script below.

```bash
./installers/install_python_deps.sh # Maybe --user flags needed
```

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/python/felicia_py.pyd`.

```bash
bazel build --config py_binding //felicia/python:felicia_py_pyd
```

#### Elsewhere

We need some python packages, please run the script below.

```bash
./installers/install_python_deps.sh # Maybe --user flags needed
```

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/python/felicia_py.so`.

```bash
bazel build --config py_binding //felicia/python:felicia_py.so
```

### wasm

**On windows it's not supported yet.**

We need some [emsdk](https://github.com/emscripten-core/emsdk.git), please run the script below. This installs `emsdk` at ` toolchain/emscripten/`.

```bash
./installers/install_emsdk.sh
```

Before building, you have to activate PATH and other environment variables in the current terminal. Let's activate them!

```bash
cd toolchain/emscripten/emsdk
source ./emsdk_env.sh
```

Also you have to do one more thing. Because `bazel` builds under virtual filesystem which is readonly, but for `emscripten` to build, we need one writable path, `.emscripten_cache.lock`.

```bash
# By default, .emscripten_cache.lock is under home directory
# Linux: /home/user/.emscripten_cache.lock
# Mac: /Users/user/.emscripten_cache.lock
# Windows: /c/Users/user/.emscripten_cache.lock

echo 'build:wasm_binding --sandbox_writable_path=/path/to/.emscripten_cache.lock' >> .bazelrc.user
```

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/wasm/felicia_wasm.js.tar`.

```bash
bazel build --config wasm_binding //felicia/wasm:felicia_wasm.js.tar
```

### node

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/js/felicia_js.node`.

#### Windows

If you don't have [nodejs](https://nodejs.org), please install it. We tested with version **10.15.3**. You can install nodejs easily with [chocolatey](https://chocolatey.org).

```bash
choco install nodejs.install --version 10.15.3
```

On windows, when linking against `node.lib`, symbols are conflited between `node.lib`(`zlib` and `openssl`) and `grpc`(`zlib` and `boringssl`). So you should build a `felicia_js.node` without `grpc` library. You can do like below.

**Please try not with `-c dbg`, because `node.lib` is provided with only release version.**

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/js/felicia_js.node`.

```bash
bazel build --config node_binding //felicia/js:felicia_js_node
```

#### Elsewhere

If you don't have [nodejs](https://nodejs.org), please install it. We tested with version **10.15.3**. The scipt below will install [nvm](https://github.com/nvm-sh/nvm) and install nodejs for you!

```bash
./installers/install_nodejs.sh
```

To generate a shared library, run the command like below. The output will be generated at `bazel-bin/felicia/js/felicia_js.node`.

```bash
bazel build --config node_binding //felicia/js:felicia_js_node
```

## How to communication ROS1 topic / service

First we need to install some python packages to serialize **ROS** messages. Run script below.

```bash
./installers/install_python_deps.sh --ros_serialization # Maybe --user flags needed
```

And then, you have to build with option `--define has_ros=true` like below.

```bash
bazel build --define has_ros=true //felicia/...
```