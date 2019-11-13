# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "master_py_cc",
    srcs = [
        "master_proxy_py.cc",
    ],
    hdrs = [
        "master_proxy_py.h",
    ],
    deps = [
        "//external:pybind11",
        "//felicia/core/master:master_proxy",
        "//felicia/python/type_conversion",
    ],
)
