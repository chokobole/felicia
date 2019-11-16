# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "channel_py_cc",
    srcs = [
        "channel_py.cc",
    ],
    hdrs = [
        "channel_py.h",
    ],
    deps = [
        "//felicia/core/channel",
        "//felicia/python/type_conversion",
    ],
)
