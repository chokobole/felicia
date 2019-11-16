# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "node_lifecycle_py_cc",
    srcs = ["node_lifecycle_py.cc"],
    hdrs = ["node_lifecycle_py.h"],
    deps = [
        "//felicia/core/node:node_lifecycle",
        "//felicia/python/type_conversion",
    ],
)
