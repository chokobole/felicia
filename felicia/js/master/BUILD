# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia.bzl", "if_win_node_binding")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "master_proxy",
    srcs = [
        "master_client.cc",
        "master_proxy_js.cc",
    ],
    hdrs = [
        "master_client.h",
        "master_proxy_js.h",
    ],
    deps = [
        "//felicia/core/master:master_proxy",
        "//felicia/core/node:topic_info_watcher_node",
        "//felicia/js/lib",
    ] + if_win_node_binding([
        "//felicia/core/master:master_client_interface",
        "//felicia/core/master/rpc:master_server_info",
    ]),
)
