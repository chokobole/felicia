# Felicia
**Cross platform**, **Secure**, **Productive** robot framework.

## How to build
In macos, you may have to add option `--cpu=darwin_x86_64 --apple_platform_type=macos`.
```
bazel build //felicia/core:core
bazel build //felicia/core:lib
bazel build //felicia/core:lib_internal
```

## How to test
Currently only support some library tests
```
bazel test //felicia/core:lib_tests
bazel test //felicia/core:platform_tests
```

### For Users
Follow this link to check [examples](felicia/examples)

