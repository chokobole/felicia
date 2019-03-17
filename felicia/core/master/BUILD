load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "client",
    srcs = ["client.cc"],
    hdrs = ["client.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/node",
        "//felicia/core/util:uuid",
    ],
)

fel_cc_library(
    name = "heart_beat_listener",
    srcs = ["heart_beat_listener.cc"],
    hdrs = ["heart_beat_listener.h"],
    deps = [
        ":master_data_proto_cc",
        "//felicia/core/channel",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "heart_beat_signaller",
    srcs = ["heart_beat_signaller.cc"],
    hdrs = ["heart_beat_signaller.h"],
    deps = [
        ":heart_beat_listener",
    ],
)

fel_proto_library(
    name = "master_data_proto",
    srcs = ["master_data.proto"],
    protodeps = ["//felicia/core/channel:channel_proto"],
    visibility = ["//felicia:internal"],
)

fel_proto_library(
    name = "master_proto",
    srcs = ["master.proto"],
    protodeps = [
        ":master_data_proto",
        "//felicia/core/channel:channel_proto",
    ],
    visibility = ["//felicia:internal"],
)

fel_cc_library(
    name = "master_client_interface",
    hdrs = ["master_client_interface.h"],
    deps = [
        ":master_data_proto_cc",
        "//felicia/core/lib",
    ],
)

fel_cc_library(
    name = "master_proxy",
    srcs = ["master_proxy.cc"],
    hdrs = ["master_proxy.h"],
    deps = [
        "//felicia/core/channel",
        "//felicia/core/master:heart_beat_signaller",
        "//felicia/core/master:topic_info_watcher",
        "//felicia/core/master/rpc:grpc_master_client",
        "//felicia/core/master/rpc:grpc_util",
        "//felicia/core/master/rpc:master_service_proto_cc",
        "//felicia/core/node",
    ],
)

fel_cc_library(
    name = "master",
    srcs = [
        "errors.h",
        "master.cc",
    ],
    hdrs = ["master.h"],
    deps = [
        ":client",
        ":heart_beat_listener",
        ":master_proto_cc",
        "//felicia/core/channel",
        "//felicia/core/lib",
        "//felicia/core/node",
    ],
)

fel_cc_library(
    name = "topic_info_watcher",
    srcs = ["topic_info_watcher.cc"],
    hdrs = ["topic_info_watcher.h"],
    deps = [
        ":master_data_proto_cc",
        "//felicia/core/channel",
        "//felicia/core/lib",
    ],
)

fel_cc_test(
    name = "master_test",
    size = "small",
    srcs = [
        "master_test.cc",
    ],
    deps = [
        ":master",
        "@com_google_googletest//:gtest_main",
    ],
)
