package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "communication",
    hdrs = [
        "communication/publisher.h",
        "communication/subscriber.h",
    ],
    copts = fel_copts(),
    deps = [
        ":client_node",
        ":master_proxy",
        "//felicia/core:channel",
    ]
)

cc_library(
    name = "client_node",
    hdrs = ["client_node.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core:lib",
        "//felicia/core:node",
        "//felicia/core/master:master_data_proto_cc",
    ]
)

cc_library(
    name = "master_proxy",
    srcs = ["master_proxy.cc"],
    hdrs = ["master_proxy.h"],
    copts = fel_copts(),
    deps = [
        ":client_node",
        "//felicia/core:channel",
        "//felicia/core/master/client:grpc_master_client",
        "//felicia/core/master/client:topic_source_watcher",
        "//felicia/core/master/rpc:grpc_util",
        "//felicia/core/master/rpc:master_service_proto_cc",
    ]
)