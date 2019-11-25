# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "command_line_interface",
    srcs = [
        "command_line_interface/flag.cc",
        "command_line_interface/flag_parser.cc",
        "command_line_interface/table_writer.cc",
        "command_line_interface/text_constants.cc",
        "command_line_interface/text_style.cc",
    ],
    hdrs = [
        "command_line_interface/flag.h",
        "command_line_interface/flag_forward.h",
        "command_line_interface/flag_parser.h",
        "command_line_interface/flag_value_traits.h",
        "command_line_interface/table_writer.h",
        "command_line_interface/text_constants.h",
        "command_line_interface/text_style.h",
    ],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "dataset",
    hdrs = [
        "dataset/dataset_loader.h",
    ],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "uuid",
    hdrs = [
        "uuid/generator.h",
    ],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "timestamp",
    srcs = ["timestamp/timestamper.cc"],
    hdrs = ["timestamp/timestamper.h"],
    deps = ["//felicia/core/lib"],
)

fel_cc_library(
    name = "util",
    deps = [
        ":command_line_interface",
        ":dataset",
        ":timestamp",
        ":uuid",
    ],
)

fel_cc_test(
    name = "util_unittests",
    size = "small",
    srcs = [
        "command_line_interface/flag_parser_unittest.cc",
        "command_line_interface/flag_unittest.cc",
    ],
    deps = [
        ":util",
        "@com_google_googletest//:gtest_main",
    ],
)
