# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_nodejs_bind.bzl", "fel_nodejs_bind_node_library")

package(default_visibility = ["//felicia:internal"])

fel_nodejs_bind_node_library(
    name = "felicia_js",
    srcs = ["felicia_js.cc"],
    deps = [
        "//felicia/js/communication",
        "//felicia/js/master:master_proxy",
        "//felicia/js/thread:main_thread",
    ],
)
