# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "node_lifecycle",
    srcs = ["node_lifecycle.cc"],
    hdrs = ["node_lifecycle.h"],
    deps = [
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "dynamic_subscribing_node",
    srcs = [
        "dynamic_subscribing_node.cc",
        "topic_info_watcher_node.cc",
    ],
    hdrs = [
        "dynamic_subscribing_node.h",
        "topic_info_watcher_node.h",
    ],
    deps = [
        ":node_lifecycle",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
    ],
)

fel_cc_library(
    name = "dynamic_publishing_node",
    srcs = ["dynamic_publishing_node.cc"],
    hdrs = ["dynamic_publishing_node.h"],
    deps = [
        ":node_lifecycle",
        "//felicia/core/communication",
        "//felicia/core/master:master_proxy",
    ],
)
