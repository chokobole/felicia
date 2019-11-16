# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "lib",
    srcs = [
        "scoped_env.cc",
        "status_js.cc",
    ],
    hdrs = [
        "scoped_env.h",
        "status_js.h",
    ],
    deps = [
        "//felicia/js/type_conversion",
    ],
)
