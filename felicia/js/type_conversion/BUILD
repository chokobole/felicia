# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "type_conversion",
    srcs = [
        "protobuf_type_convertor.cc",
    ],
    hdrs = [
        "protobuf_type_convertor.h",
        "type_convertor.h",
        "type_convertor_forward.h",
        "typed_call.h",
    ],
    deps = [
        "//felicia/core/lib",
        "@node_addon_api",
    ],
)
