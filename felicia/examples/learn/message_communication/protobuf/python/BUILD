package(default_visibility = ["//visibility:private"])

py_binary(
    name = "node_creator",
    srcs = [
        "node_creator.py",
        "simple_publishing_node.py",
        "simple_subscribing_node.py",
    ],
    main = "node_creator.py",
    deps = [
        "//felicia/examples/learn/message_communication/common/python:import_order_resolver",
        "//felicia/examples/learn/message_communication/common/python:node_create_flag",
        "//felicia/examples/learn/message_communication/protobuf:message_spec_proto_py",
        "//felicia/python:felicia_py",
    ],
)
