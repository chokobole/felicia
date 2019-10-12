# Felicia

[![Build Status](https://travis-ci.com/chokobole/felicia.svg?token=uWEvhLXsK9nuPxhDRPic&branch=master)](https://travis-ci.com/chokobole/felicia)

Felicia is the chromium based robot framework like [ROS](https://www.ros.org/), a collections of tools and libraries to help developers build **fast**, **light** and **secure** robot software. Currently it is under development mostly alone, I think it is still useful for like Robot-vision developer who needs to deliver camera images fast or web developers who wants to subscribe messages by WebSocket without any performance drop. I hope someday it be spread through robot industry and academia so that I want many developers to be engaged with it! If you have any trouble, leave issues or contact to me by email!

**Key** features:

* Cross Platform.
* Support Protobuf / ROS Message.
* Provide C++ / Python APIs.
* Provide different communication channels (TCP / UDP / Shared Memory / WebSocket / Unix Domain Socket).
* Provide some of sensor APIs.
  * Realsense D435i
  * ZED Stereo Camrea
  * RPLidar
  * UVC compatible camera
* Provide command-line-interface.
* Provide visualization tool with browser.
* Compatible with ROS1 topic protocol.

**TODO** feautures:

* Decentralize nodes.
* Support TLS communicaiton.
* Support OS-layer security.
* Provide more channels such as Bluetooth, QUIC.
* Provide more sensor APIs such as michrophone.
* Provide simulator.
* Compatible with ROS1 service protocol.
* Compatible with ROS2 topic / service protocol.
* Support Raspberry Pi 4.
* Robot specific features such as SLAM.

## Prerequisites

We use [bazel](https://www.bazel.build/) as a build tool. So please download it! To sync with window build, we fix the virstion to `0.25.0`.
On linux and mac, simply you can do like below!

```bash
./installers/install_bazel.sh
```

Also we need `python` and some dependencies such as `numpy`. Currently we tested on python3. Our suggestion is using `pipenv`.

```bash
pipenv --three
./installers/install_python_deps.sh
```

Or you can set environment variable `PYTHON_BIN_PATH` to `/path/to/python/`.

Lastly we need `nodejs` for nodejs binding. If you are working on linux or mac, then you can install it by `nvm`. On windows, you can install it by `choco`.

```bash
# Install nodejs using nvm on linux or mac
./installers/install_nodejs.sh

# install nodejs using choco on windows
choco install nodejs.install --version 10.15.3
```

### For Windows developers

For only window developers, you have to do more. You have to download llvm from [llvm.org](http://llvm.org/builds/) also. Because you need `clang-cl` for compiler. And then to change default compiler for bazel, you have to set `USE_CLANG_CL` to `1` and `BAZEL_LLVM` to `/path/to/llvm` if you install llvm at not regular location. This feature is a quite new, so you need a recent released one. We tested on `0.25.0`.

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

## How to set up on Docker

```bash
docker build . -t felicia -f docker/Dockerfile.ubuntu
```

You can validate your work on docker with below.

```bash
python3 scripts/docker_exec.py bazel build //felicia/...
```

## For Users

Follow this link to check [examples](felicia/examples)

## For Developers

After you write the any BUILD, or *.bzl, don't forget to run buildifier!

```bash
bazel run //:buildifier
```
