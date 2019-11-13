# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia_wasm.bzl", "fel_wasm_binary")

fel_wasm_binary(
    name = "felicia_wasm.js",
    srcs = ["felicia_wasm.cc"],
    copts = [
        "--bind",
    ],
    linkopts = [
        "--bind",
        "-s ENVIRONMENT='worker'",
        "-s ALLOW_MEMORY_GROWTH=1",
    ],
    deps = [
        "//external:libyuv",
    ],
)
