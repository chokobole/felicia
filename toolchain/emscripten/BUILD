# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load(":cc_toolchain_config.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

cc_toolchain_suite(
    name = "emscripten",
    toolchains = {
        "k8": ":wasm_toolchain",
        "darwin_x86_64": ":wasm_toolchain",
        "x64_windows": ":wasm_toolchain",
    },
)

filegroup(
    name = "emsdk",
    srcs = glob(["emsdk/**/*"]),
)

filegroup(
    name = "all",
    srcs = [
        "emar.bat",
        "emar.sh",
        "emcc.bat",
        "emcc.sh",
        ":emsdk",
    ],
)

cc_toolchain(
    name = "wasm_toolchain",
    all_files = ":all",
    compiler_files = ":all",
    dwp_files = ":empty",
    linker_files = ":all",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain_config = ":wasm_toolchain_config",
    toolchain_identifier = "wasm-toolchain",
)

cc_toolchain_config(name = "wasm_toolchain_config")
