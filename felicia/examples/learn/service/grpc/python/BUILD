package(default_visibility = ["//visibility:private"])

py_binary(
    name = "grpc_node_creator",
    srcs = [
        "grpc_node_creator.py",
        "grpc_service_flag.py",
    ],
    main = "grpc_node_creator.py",
    deps = [
        "//felicia/examples/learn/service/common/python:service_create_flag",
        "//felicia/examples/learn/service/grpc:simple_service_proto_py",
        "//felicia/python:felicia_py",
        "//felicia/python:import_order_resolver",
    ],
)
