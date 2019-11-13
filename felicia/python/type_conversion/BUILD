# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "type_conversion",
    srcs = [
        "util.cc",
    ],
    hdrs = [
        "callback.h",
        "callback_forward.h",
        "callback_holder.h",
        "protobuf.h",
        "util.h",
    ],
    deps = [
        "//external:pybind11",
        "//felicia/core/lib",
        "//felicia/python/message",
    ],
)
