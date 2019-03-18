load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//visibility:private"])

py_library(
    name = "node_create_flag",
    srcs = ["node_create_flag.py"],
    deps = [
        "//felicia/python/core/util/command_line_interface:flag",
        "//felicia/python/core/util/command_line_interface:text_style",
    ],
)

fel_pybind_py_library(
    name = "communication",
    cc_deps = [
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
        "//felicia/python:communication_py_helper",
        "//felicia/python/core:type_conversion",
    ],
    py_deps = [
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_py",
        "//felicia/python:communication",
    ],
)

py_library(
    name = "simple_publishing_node",
    srcs = ["simple_publishing_node.py"],
    deps = [
        ":communication",
        "//felicia/core/master:master_data_proto_py",
        "//felicia/python/core/node",
    ],
)

py_library(
    name = "simple_subscribing_node",
    srcs = ["simple_subscribing_node.py"],
    deps = [
        ":communication",
        "//felicia/core/master:master_data_proto_py",
        "//felicia/python/core/node",
    ],
)

py_binary(
    name = "node_creator",
    srcs = ["node_creator.py"],
    main = "node_creator.py",
    deps = [
        ":node_create_flag",
        ":simple_publishing_node",
        ":simple_subscribing_node",
        "//felicia/python:master_proxy",
    ],
)
