# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_proto.bzl", "fel_proto_library")
load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//visibility:private"])

fel_proto_library(
    name = "test_proto",
    srcs = ["test.proto"],
    default_header = True,
    visibility = ["//visibility:public"],
)

fel_pybind_py_library(
    name = "protobuf_py",
    cc_deps = [
        ":test_proto_cc",
        "//felicia/python/type_conversion",
    ],
    cc_srcs = [
        "protobuf_py.cc",
    ],
    py_deps = [
        ":test_proto_py",
    ],
)

py_binary(
    name = "protobuf_benchmark",
    testonly = 1,
    srcs = [
        "protobuf_benchmark.py",
    ],
    main = "protobuf_benchmark.py",
    tags = ["benchmark"],
    deps = [
        ":protobuf_py",
    ],
)
