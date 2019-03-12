package(default_visibility = ["//felicia:internal"])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "py_flag_parser_delegate",
    hdrs = [
        "py_flag_parser_delegate.h",
    ],
    copts = fel_copts(),
    deps = [
        "@pybind11",
        "//felicia/core/util:command_line_interface"
    ]
)

cc_binary(
    name = "flag.so",
    srcs = [
        "flag_py.cc",
    ],
    copts = fel_copts(),
    linkshared = 1,
    linkstatic = 1,
    deps = [
        ":py_flag_parser_delegate",
        "@pybind11",
        "//felicia/core/util:command_line_interface"
    ],
)

py_library(
    name = "command_line_util",
    data = [":flag.so"],
    srcs = [
        "simple_flag_parser_delegate.py",
        "text_style.py",
    ],
    imports = ["."],
)
