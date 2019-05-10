load("//bazel:felicia_pybind.bzl", "fel_pybind_py_library")

package(default_visibility = ["//visibility:private"])

fel_pybind_py_library(
    name = "felicia_py",
    cc_deps = [
        "//felicia/python:basic_felicia_py_cc",
    ],
    cc_srcs = ["communication_py.cc"],
    py_deps = [
        "//felicia/python:basic_felicia_py_py",
    ],
)

py_binary(
    name = "node_creator",
    srcs = [
        "camera_publishing_node.py",
        "camera_subscribing_node.py",
        "node_creator.py",
    ],
    main = "node_creator.py",
    deps = [
        ":felicia_py",
        "//felicia/examples/learn/message_communication/common/python:camera_flag",
        "//felicia/examples/learn/message_communication/common/python:import_order_resolver",
    ],
)
