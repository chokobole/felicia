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
    protodeps = [":master_data_proto"]
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
    srcs = ["master.cc"],
    hdrs = ["master.h"],
    copts = fel_copts(),
    deps = [
        ":master_proto_cc",
        ":server_node",
        "//felicia/core:channel",
        "//felicia/core:lib",
        "//felicia/core:node",
    ]
)

cc_library(
    name = "server_node",
    srcs = ["server_node.cc"],
    hdrs = ["server_node.h"],
    copts = fel_copts(),
    deps = [
        ":master_data_proto_cc",
        "//felicia/core:lib",
    ]
)