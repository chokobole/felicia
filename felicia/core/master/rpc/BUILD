package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

load(
    "//felicia/core:platform/build_config.bzl",
    "fel_proto_library_cc",
)

cc_library(
    name = "grpc_client_cq_tag",
    hdrs = ["grpc_client_cq_tag.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core:lib",
    ]
)

cc_library(
    name = "grpc_async_client_call",
    hdrs = ["grpc_async_client_call.h"],
    copts = fel_copts(),
    deps = [
        ":grpc_client_cq_tag",
        ":grpc_util",
        "//felicia:grpc++",
        "//felicia/core:lib",
    ]
)

cc_library(
    name = "grpc_call",
    hdrs = ["grpc_call.h"],
    copts = fel_copts(),
    deps = [
        "//felicia:grpc++",
        "//felicia/core:lib",
    ]
)

cc_library(
    name = "grpc_util",
    srcs = ["grpc_util.cc"],
    hdrs = ["grpc_util.h"],
    copts = fel_copts(),
    deps = [
        "//felicia:grpc++",
        "//felicia/core:lib",
        "//felicia/core:platform_net",
    ],
)

fel_proto_library_cc(
    name = "master_service_proto",
    srcs = ["master_service.proto"],
    has_services = 1,
    cc_grpc_version = 1,
    protodeps = ["//felicia/core/master:master_proto"],
    visibility = ["//felicia:internal"],
)

cc_library(
    name = "grpc_master_service",
    srcs = ["grpc_master_service.cc"],
    hdrs = ["grpc_master_service.h"],
    copts = fel_copts(),
    deps = [
        ":grpc_call",
        ":grpc_util",
        ":master_service_proto_cc",
        "//felicia/core:lib",
        "//felicia/core/master:master",
    ],
)

cc_library(
    name = "grpc_server",
    srcs = ["grpc_server.cc"],
    hdrs = ["grpc_server.h"],
    copts = fel_copts(),
    deps = [
        ":grpc_master_service",
        ":grpc_util",
        "//felicia:grpc++",
        "//felicia/core/master:master",
    ],
)

cc_binary(
    name = "grpc_server_main",
    srcs = ["grpc_server_main.cc"],
    copts = fel_copts(),
    deps = [
        ":grpc_server",
    ],
)

cc_library(
    name = "scoped_grpc_request",
    hdrs = ["scoped_grpc_request.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core:lib",
        "//felicia/core/master:master_client_interface",
    ]
)