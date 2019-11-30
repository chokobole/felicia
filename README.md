

![Felicia Logo](docs/resources/felicia-logo.png)

[![Build Status](https://travis-ci.com/chokobole/felicia.svg?token=uWEvhLXsK9nuPxhDRPic&branch=master)](https://travis-ci.com/chokobole/felicia)

## Overview

Felicia is the chromium based robot framework like [ROS](https://www.ros.org/), a collections of tools and libraries to help developers build **fast**, **light** and **secure** robot software. Currently it is under development mostly alone, I think it is still useful for like Robot-vision developer who needs to deliver camera images fast or web developers who wants to subscribe messages by WebSocket without any performance drop. I hope someday it be spread through robot industry and academia so that I want many developers to be engaged with it! If you have any trouble, leave issues or contact to me by email!

**Key** features:

* Cross Platform.
* Support Protobuf / ROS Message.
* Use grpc.
* Provide C++ / Python APIs.
* Provide different communication channels (TCP / UDP / Shared Memory / WebSocket / Unix Domain Socket).
* Provide some of sensor APIs.
  * Realsense D435i
  * ZED Stereo Camrea
  * RPLidar
  * UVC compatible camera
* Provide command-line-interface.
* Provide visualization tool with browser.
* Compatible with ROS1 topic / service protocol.

**TODO** feautures:

* Decentralize nodes.
* Support TLS communicaiton.
* Support OS-layer security.
* Provide more channels such as Bluetooth, QUIC.
* Provide more sensor APIs such as michrophone.
* Provide simulator.
* Compatible with ROS2 topic / service protocol.
* Support Raspberry Pi 4.
* Robot specific features such as SLAM.

## Prerequisites

We use [bazel](https://www.bazel.build/) as a build tool. So please download and install it!

```bash
# Install bazel on linux or mac
./installers/install_bazel.sh

# Install bazel on windows
choco install bazel --version 0.25.0
```

Also we need `python` and some dependencies such as `numpy`. Currently we tested on python3. Our suggestion is using `pipenv`.

```bash
pipenv --three
./installers/install_python_deps.sh
```

To build, either `python` or environment variable `PYTHON_BIN_PATH` points to `/path/to/python3` and either `python2` or environment variable `PYTHON2_BIN_PATH` points to `/path/to/python2`.

Lastly we need `nodejs` for nodejs binding. If you are working on linux or mac, then you can install it by `nvm`. On windows, you can install it by `choco`.

```bash
# Install nodejs using nvm on linux or mac
./installers/install_nodejs.sh

# Install nodejs using choco on windows
choco install nodejs.install --version 10.15.3
```

### For Windows developers

For only window developers, you have to do more. You have to download llvm from [llvm.org](http://llvm.org/builds/) also. Because you need `clang-cl` for compiler. And then to change default compiler for bazel, you have to set `USE_CLANG_CL` to `1` and `BAZEL_LLVM` to `/path/to/llvm` if you install llvm at not regular location.

## How to build

```bash
bazel build //felicia/...
```

### For Macos developers

Currently, in bazel, `--cpu` option's default value is `darwin`, which doesn't work on our project(If your cpu is 64bit.). You have to manually set `darwin_x86_64` on every command like below.

```bash
bazel build --cpu darwin_x86_64 //felicia/...
```

Also in order to leave debugging symbol on mac, you have to add config `apple_debug` like below. If you aren't working on mac, it's enough to set `-c dbg`.

```bash
bazel build --cpu darwin_x86_64 --config apple_debug //felicia/...
```

## How to test

```bash
bazel test //felicia/...
```

## How to package

```bash
# For linux
# generate library, check out bazel-bin/felicia/libfelicia.so
bazel build --define framework_shared_object=true //felicia:libfelicia.so

# For windows
bazel build --define framework_shared_object=true //felicia:felicia.dll
bazel build --define framework_shared_object=true //felicia:felicia_lib

# For mac
bazel build --cpu darwin_x86_64 --define framework_shared_object=true //felicia:libfelicia.dylib

# generate tar for headers, check out bazel-bin/felicia/felicia_hdrs.tar
bazel build --define framework_shared_object=true //felicia:felicia_hdrs
```

Then you can take those two outputs and put library and extract headers wherever you want. You can check out examples [here](https://github.com/chokobole/felicia-examples).


## How to set up on Docker

```bash
docker build . -t felicia -f docker/Dockerfile.ubuntu.xenial
```

You can validate your work on docker with below.

```bash
python3 tools/docker_exec.py bazel build //felicia/...
```

## For Users

Follow this link to check [felicia-examples](https://github.com/chokobole/felicia-examples).

## For Developers

After you write the any BUILD, or *.bzl, don't forget to run buildifier!

```bash
bazel run //:buildifier
```
