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

fel_proto_library_cc(
    name = "master_data_proto",
    srcs = ["master_data.proto"],
    visibility = ["//felicia:internal"],
    protodeps = ["//felicia/core:channel_proto"],
)

fel_proto_library_cc(
    name = "master_proto",
    srcs = ["master.proto"],
    visibility = ["//felicia:internal"],
    protodeps = [
        ":master_data_proto",
        "//felicia/core:channel_proto",
    ]
)

cc_library(
    name = "master_client_interface",
    hdrs = ["master_client_interface.h"],
    copts = fel_copts(),
    deps = [
        ":master_data_proto_cc",
        "//felicia/core:lib",
    ]
)

cc_library(
    name = "master",
    srcs = [
        "errors.h",
        "master.cc",
    ],
    hdrs = ["master.h"],
    copts = fel_copts(),
    deps = [
        ":master_proto_cc",
        ":heart_beat_listener",
        "//felicia/core:channel",
        "//felicia/core:lib",
        "//felicia/core:node",
        "//felicia/core/master/client:client"
    ]
)

cc_library(
    name = "task_runner_interface",
    hdrs = ["task_runner_interface.h"],
    copts = fel_copts(),
    deps = [
        "//felicia/core:lib"
    ]
)

cc_library(
    name = "topic_info_watcher",
    srcs = ["topic_info_watcher.cc"],
    hdrs = ["topic_info_watcher.h"],
    copts = fel_copts(),
    deps = [
        ":master_data_proto_cc",
        ":task_runner_interface",
        "//felicia/core:lib",
        "//felicia/core:channel",
    ]
)

cc_library(
    name = "heart_beat_listener",
    srcs = ["heart_beat_listener.cc"],
    hdrs = ["heart_beat_listener.h"],
    copts = fel_copts(),
    deps = [
        ":master_data_proto_cc",
        ":task_runner_interface",
        "//felicia/core:lib",
        "//felicia/core:channel",
    ]
)

cc_library(
    name = "heart_beat_signaller",
    srcs = ["heart_beat_signaller.cc"],
    hdrs = ["heart_beat_signaller.h"],
    copts = fel_copts(),
    deps = [
        ":heart_beat_listener",
    ]
)

cc_test(
    name = "master_test",
    size = "small",
    srcs = [
        "master_test.cc"
    ],
    copts = fel_copts(),
    linkstatic = 1, # Failed at macos when built with opt
    deps = [
        ":master",
        "//felicia/core:test_main",
    ]
)