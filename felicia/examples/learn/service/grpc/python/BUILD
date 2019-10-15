package(default_visibility = ["//visibility:private"])

py_binary(
    name = "grpc_node_creator",
    srcs = [
        "grpc_client_node.py",
        "grpc_node_creator.py",
        "grpc_server_node.py",
    ],
    main = "grpc_node_creator.py",
    deps = [
        "//felicia/examples/learn/service/common/python:simple_service_flag",
        "//felicia/examples/learn/service/grpc:simple_service_proto_py",
        "//felicia/python:felicia_py",
        "//felicia/python:import_order_resolver",
    ],
)
