package(default_visibility = ["//visibility:private"])

py_binary(
    name = "node_creator",
    srcs = [
        "camera_publishing_node.py",
        "camera_subscribing_node.py",
        "node_creator.py",
    ],
    main = "node_creator.py",
    deps = [
        "//felicia/examples/learn/message_communication/common/python:camera_flag",
        "//felicia/examples/learn/message_communication/common/python:import_order_resolver",
        "//felicia/python:felicia_py",
    ],
)
