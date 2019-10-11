package(default_visibility = ["//visibility:private"])

py_binary(
    name = "protobuf_node_creator",
    srcs = [
        "protobuf_node_creator.py",
        "protobuf_publishing_node.py",
        "protobuf_subscribing_node.py",
    ],
    main = "protobuf_node_creator.py",
    deps = [
        "//felicia/examples/learn/topic/common/python:import_order_resolver",
        "//felicia/examples/learn/topic/common/python:topic_create_flag",
        "//felicia/examples/learn/topic/protobuf:simple_message_proto_py",
        "//felicia/python:felicia_py",
    ],
)
