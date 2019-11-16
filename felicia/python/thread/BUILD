# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "main_thread_py_cc",
    srcs = ["main_thread_py.cc"],
    hdrs = ["main_thread_py.h"],
    deps = [
        "//felicia/core/thread:main_thread",
        "//felicia/python/type_conversion",
    ],
)
