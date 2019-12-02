# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia.bzl", "if_py_binding")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "message",
    srcs = [
        "message_util.cc",
    ],
    hdrs = [
        "message_util.h",
    ],
    deps = if_py_binding([
        "//external:pybind11",
        "//felicia/core/message",
    ]),
)
