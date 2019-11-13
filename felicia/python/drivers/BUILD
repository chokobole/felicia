# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "drivers_py_cc",
    srcs = [
        "camera_py.cc",
        "drivers_py.cc",
    ],
    hdrs = [
        "camera_py.h",
        "drivers_py.h",
    ],
    deps = [
        "//external:pybind11",
        "//felicia/drivers/camera",
        "//felicia/python/type_conversion",
    ],
)
