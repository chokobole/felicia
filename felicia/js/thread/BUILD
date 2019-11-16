# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "main_thread",
    srcs = ["main_thread_js.cc"],
    hdrs = ["main_thread_js.h"],
    deps = [
        "//felicia/core/thread:main_thread",
        "//felicia/js/type_conversion",
    ],
)
