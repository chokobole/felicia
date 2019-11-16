# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication",
    srcs = ["settings_js.cc"],
    hdrs = ["settings_js.h"],
    deps = [
        "//felicia/core/communication",
        "//felicia/js/type_conversion",
    ],
)
