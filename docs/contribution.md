# Contribution

Contributions are always welcome! You can report issues or bugs, suggest features or even implement and send pull request!

## Contents
- [Contribution](#contribution)
  - [Contents](#contents)
  - [Prerequisites](#prerequisites)
  - [Coding Style](#coding-style)
    - [C++](#c)
    - [Bazel](#bazel)
  - [Unit testing](#unit-testing)
  - [Update Protobuf](#update-protobuf)
  - [C++ Development tips](#c-development-tips)
    - [IDE Support](#ide-support)
      - [vscode](#vscode)
    - [Memory isssue](#memory-isssue)
    - [Debug on mac](#debug-on-mac)

## Prerequisites

To install python dependencies for development, please run the script below.

```bash
./installers/install_python_deps.sh --dev # Maybe --user flags needed
```

## Coding Style

We follow [google coding style](http://google.github.io/styleguide/).

### C++

For the c++ developers, before sending pull request, please do `git-clang-format`.

```bash
git-clang-format
```

### Bazel

For the developers working on bazel, before sending pull request, please run `buildifier`.

```bash
bazel run //:buildifier
```

## Unit testing

```bash
bazel test //felicia/...
```

## Update Protobuf

For the developers updating protobuf, before sending pull request, please update `protobuf` to [felicia_proto_bundle.json](/felicia-viz/modules/proto/src/felicia_proto_bundle.json) and appropriate .ts file under [messages](/felicia-viz/modules/proto/src/messages).

```bash
cd felicia-viz/modules/proto
npx pbjs -t json ../../../felicia/core/protobuf/*.proto ../../../felicia/drivers/**/*.proto ../../../felicia/map/**/*.proto ../../../felicia/slam/**/*.proto > src/felicia_proto_bundle.json
```

Also don't forget [formatting](/felicia-viz/docs/contribution.md#coding-style).

## C++ Development tips

If you are working on the c++ code, there are some useful tips you have to check out!

### IDE Support

#### vscode

To make use of intellisense, you have to configure sttings in `c_cpp_properties.json` like below.

```js
{
  "env": {
    "bazel_output_base": "/path/to/output_base" // You can obtain by command `bazel info output_base`
  },
  "configuration": [
    {
      // ...
      "includePath": [
        "bazel-genfiles",
        "${bazel_output_base}/execroot/felicia",
        "${bazel_output_base}/execroot/felicia/third_party/chromium",
        "${bazel_output_base}/execroot/felicia/external/com_google_protobuf/src/",
        "${bazel_output_base}/execroot/felicia/external/com_google_googletest/googletest/include/",
        "${bazel_output_base}/execroot/felicia/external/com_google_googletest/googlemock/include/",
        "${bazel_output_base}/execroot/felicia/external/com_github_grpc_grpc/include/",
        // ...
      ],
    }
  ]
}
```

### Memory isssue

* Address Sanitizer(asan)(--define asan=true): Compile and Link using asan.

### Debug on mac

In order to leave debugging symbol on mac, you have to add config `apple_debug` like below.

```bash
bazel build --config apple_debug //felicia/...
```