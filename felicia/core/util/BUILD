package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "command_line_interface",
    srcs = [
        "command_line_interface/flag_forward.h",
        "command_line_interface/flag_parser.cc",
        "command_line_interface/flag.cc",
        "command_line_interface/flag_value_traits.h",
        "command_line_interface/table_writer.cc",
    ],
    hdrs = [
        "command_line_interface/flag.h",
        "command_line_interface/flag_parser.h",
        "command_line_interface/table_writer.h",
        "command_line_interface/text_style.h",
    ],
    copts = fel_copts(),
    deps = ["//felicia/core:lib"]
)

cc_library(
    name = "util",
    deps = [
        ":command_line_interface",
    ]
)

cc_test(
    name = "util_tests",
    size = "small",
    srcs = [
        "command_line_interface/flag_test.cc",
        "command_line_interface/flag_parser_test.cc",
    ],
    copts = fel_copts(),
    deps = [
        ":util",
        "//felicia/core:test_main",
    ]
)