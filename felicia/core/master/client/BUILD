package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "grpc_master_client",
    srcs = ["grpc_master_client.cc"],
    hdrs = ["grpc_master_client.h"],
    copts = fel_copts(),
    deps = [
        "//felicia:grpc++",
        "//felicia/core:lib",
        "//felicia/core/master:master_client_interface",
        "//felicia/core/master/rpc:grpc_async_client_call",
        "//felicia/core/master/rpc:master_service_proto_cc",
    ],
)

cc_library(
    name = "topic_source_watcher",
    srcs = ["topic_source_watcher.cc"],
    hdrs = ["topic_source_watcher.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core:lib",
        "//felicia/core:channel",
        "//felicia/core/master:master_data_proto_cc",
        "//felicia/core/master/rpc:grpc_util",
    ]
)