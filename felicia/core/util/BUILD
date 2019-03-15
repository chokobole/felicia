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
        "command_line_interface/flag_forward.h",
        "command_line_interface/flag_parser.cc",
        "command_line_interface/flag_value_traits.h",
        "command_line_interface/table_writer.cc",
    ],
    hdrs = [
        "command_line_interface/flag.h",
        "command_line_interface/flag_parser.h",
        "command_line_interface/table_writer.h",
        "command_line_interface/text_style.h",
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

fel_cc_test(
    name = "util_tests",
    size = "small",
    srcs = [
        "command_line_interface/flag_parser_test.cc",
        "command_line_interface/flag_test.cc",
    ],
    deps = [
        ":command_line_interface",
        ":uuid",
        "@com_google_googletest//:gtest_main",
    ],
)
