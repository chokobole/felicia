load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//visibility:private"])

fel_pybind_py_library(
    name = "felicia_py",
    cc_deps = [
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_cc",
        "//felicia/python:basic_felicia_py_cc",
    ],
    cc_srcs = ["communication_py.cc"],
    py_deps = [
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_py",
        "//felicia/python:basic_felicia_py_py",
    ],
)

py_binary(
    name = "node_creator",
    srcs = [
        "node_creator.py",
        "simple_publishing_node.py",
        "simple_subscribing_node.py",
    ],
    main = "node_creator.py",
    deps = [
        ":felicia_py",
        "//felicia/examples/learn/message_communication/common/python:node_create_flag",
    ],
)
