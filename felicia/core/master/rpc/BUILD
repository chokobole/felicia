load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library_cc")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "grpc_async_client_call",
    hdrs = ["grpc_async_client_call.h"],
    deps = [
        ":grpc_client_cq_tag",
        ":grpc_util",
        "//felicia:grpc++",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "grpc_call",
    hdrs = ["grpc_call.h"],
    deps = [
        "//felicia:grpc++",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "grpc_client_cq_tag",
    hdrs = ["grpc_client_cq_tag.h"],
    deps = [
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "grpc_master_client",
    srcs = ["grpc_master_client.cc"],
    hdrs = ["grpc_master_client.h"],
    deps = [
        ":grpc_async_client_call",
        ":master_service_proto_cc",
        "//felicia:grpc++",
        "//felicia/core/lib",
        "//felicia/core/master:master_client_interface",
    ],
)

fel_proto_library_cc(
    name = "master_service_proto",
    srcs = ["master_service.proto"],
    has_services = 1,
    cc_grpc_version = 1,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//felicia:internal"],
)

fel_cc_binary(
    name = "grpc_server_main",
    srcs = [
        "grpc_master_service.cc",
        "grpc_master_service.h",
        "grpc_server.cc",
        "grpc_server.h",
        "grpc_server_main.cc",
    ],
    deps = fel_shared_library() + [
        ":grpc_call",
        ":grpc_util",
        ":master_service_proto_cc",
        "//felicia/core:felicia_init",
        "//felicia/core/master",
    ],
)

fel_cc_library(
    name = "grpc_util",
    srcs = ["grpc_util.cc"],
    hdrs = ["grpc_util.h"],
    deps = [
        ":grpc_info",
        "//felicia:grpc++",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "grpc_info",
    srcs = ["grpc_info.cc"],
    hdrs = ["grpc_info.h"],
    deps = ["//felicia/core/lib"],
)
