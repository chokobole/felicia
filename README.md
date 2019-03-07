# Felicia

[![Build Status](https://travis-ci.com/chokobole/felicia.svg?token=uWEvhLXsK9nuPxhDRPic&branch=master)](https://travis-ci.com/chokobole/felicia)

**Cross platform**, **Secure**, **Productive** robot framework.

## Prerequisites

We use [bazel](https://www.bazel.build/) as a build tool. So please download it! To sync with window build, we fix the virstion to `0.20.0`.
On linux and mac, simply you can do like below!

```bash
./installers/install_bazel.sh
```

### For Windows developers

For only window developers, you have to do more. You have to download llvm from [llvm.org](http://llvm.org/builds/) also. Because you need `clang-cl` for compiler. And then to change default compiler for bazel, you have to set `USE_CLANG_CL` to `1` and `BAZEL_LLVM` to `/path/to/llvm`. This feature is a quite new, so you need a recent released one. We tested on `0.20.0`.

## How to build

```bash
python3 scripts/felicia.py build [target]
```

## How to test

```bash
python3 scripts/felicia.py build [target]
```

## How to set up on Docker

```bash
docker build . -t felicia -f docker/Dockerfile.ubuntu
```

You can validate your work on docker with below.

```bash
python3 scripts/felicia.py --with_docker build all --with_test
```

## For Users
Follow this link to check [examples](felicia/examples)

