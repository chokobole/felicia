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
        ":master_proxy",
        "//felicia/core:channel",
        "//felicia/core:node",
    ]
)

cc_library(
    name = "master_proxy",
    srcs = ["master_proxy.cc"],
    hdrs = ["master_proxy.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core:channel",
        "//felicia/core:node",
        "//felicia/core/master:heart_beat_signaller",
        "//felicia/core/master:topic_info_watcher",
        "//felicia/core/master/client:grpc_master_client",
        "//felicia/core/master/rpc:grpc_util",
        "//felicia/core/master/rpc:master_service_proto_cc",
    ]
)