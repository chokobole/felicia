# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load("//bazel:felicia.bzl", "if_static")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_binary",
    "fel_cc_library",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library_cc")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "master_method_list",
    hdrs = ["master_method_list.h"],
)

fel_cc_library(
    name = "master_client",
    srcs = ["master_client.cc"],
    hdrs = ["master_client.h"],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/master:master_client_interface",
        "//felicia/core/rpc",
    ] + if_static(
        [
            ":master_service_proto_cc",
        ],
        [
            ":master_service_proto_cc_impl",
        ],
    ),
)

fel_proto_library_cc(
    name = "master_service_proto",
    srcs = ["master_service.proto"],
    has_services = 1,
    cc_grpc_version = 1,
    default_header = True,
    export_proto = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//felicia:internal"],
)

fel_cc_binary(
    name = "master_server_main",
    srcs = [
        "master_server.cc",
        "master_server.h",
        "master_server_main.cc",
        "master_service.cc",
        "master_service.h",
    ],
    deps = [
        ":master_server_info",
        ":master_service_proto_cc",
        "//felicia/core:felicia_init",
        "//felicia/core/master",
        "//felicia/core/rpc",
    ],
)

fel_cc_library(
    name = "master_server_info",
    srcs = ["master_server_info.cc"],
    hdrs = ["master_server_info.h"],
    deps = ["//felicia/core/lib"],
)
