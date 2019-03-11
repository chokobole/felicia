# Copyright 2015 The TensorFlow Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ==============================================================================
# Modifications copyright (C) 2019 felicia

package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)
load(
    "//felicia/core:platform/build_config.bzl",
    "fel_additional_lib_deps",
    "fel_additional_lib_hdrs",
    "fel_additional_lib_srcs",
    "fel_platform_files",
    "fel_proto_library",
    "if_static",
)

##########################################################################################
########################################## Libs ##########################################
##########################################################################################

fel_proto_library(
    name = "error_codes_proto",
    srcs = ["lib/error/error_codes.proto"],
)

cc_library(
    name = "platform_net",
    srcs = fel_platform_files([
        "net_util.cc",
    ]),
    hdrs = [
        "platform/net_util.h",
    ],
    copts = fel_copts(),
    deps = [
        ":lib",
        "//third_party/chromium/net",
    ],
)

fel_proto_library(
    name = "channel_proto",
    srcs = ["channel/channel.proto"],
    visibility = ["//felicia:internal"],
)

cc_library(
    name = "channel",
    srcs = [
        "channel/channel.cc",
        "channel/channel_base.cc",
        "channel/channel_base.h",
        "channel/tcp_channel.h",
        "channel/tcp_channel_base.h",
        "channel/tcp_client_channel.cc",
        "channel/tcp_client_channel.h",
        "channel/tcp_server_channel.cc",
        "channel/tcp_server_channel.h",
        "channel/udp_channel.h",
        "channel/udp_channel_base.cc",
        "channel/udp_channel_base.h",
        "channel/udp_client_channel.cc",
        "channel/udp_client_channel.h",
        "channel/udp_server_channel.cc",
        "channel/udp_server_channel.h",
    ],
    hdrs = [
        "channel/channel.h",
        "channel/channel_factory.h",
    ],
    copts = fel_copts(),
    deps = [
        ":channel_proto_cc",
        ":lib",
        ":message",
        ":platform_net",
    ],
)

cc_library(
    name = "node",
    srcs = [
        "node/node.cc",
    ],
    hdrs = [
        "node/node.h",
        "node/node_lifecycle.h",
    ],
    copts = fel_copts(),
    deps = [
        ":lib",
        "//felicia/core/master:master_proto_cc",
        "//felicia/core/util:uuid",
    ],
)

cc_library(
    name = "message",
    srcs = [
        "message/header.cc",
    ],
    hdrs = [
        "message/header.h",
        "message/message.h",
    ],
    copts = fel_copts(),
    deps = [
        ":lib",
    ],
)

LIB_INTERNAL_PRIVATE_HEADERS = glob(
    [
        "lib/**/*.h",
        "platform/*.h",
    ],
    exclude = [
        "platform/net_util.h",
    ],
)

LIB_INTERNAL_PUBLIC_HEADERS = fel_additional_lib_hdrs(
    exclude = [
        "platform/**/net_util.h",
    ],
)

cc_library(
    name = "lib_internal",
    srcs = LIB_INTERNAL_PRIVATE_HEADERS,
    hdrs = LIB_INTERNAL_PUBLIC_HEADERS,
    copts = fel_copts(),
    linkopts = select({
        "//felicia:windows": [],
        "//conditions:default": [
            "-lpthread",
        ],
    }),
    deps = fel_additional_lib_deps() + if_static([
        ":lib_internal_impl",
    ]),
)

cc_library(
    name = "lib_internal_impl",
    srcs = LIB_INTERNAL_PRIVATE_HEADERS + glob(
        [
            "lib/**/*.cc",
            "platform/*.cc",
        ],
        exclude = [
            "**/*test*",
            "**/*benchmark*",
            "platform/**/net_util.cc",
        ],
    ) + fel_additional_lib_srcs(
        exclude = [
            "**/*test*",
            "**/*benchmark*",
            "platform/**/net_util.cc",
        ],
    ),
    hdrs = LIB_INTERNAL_PUBLIC_HEADERS,
    copts = fel_copts(),
    deps = [":error_codes_proto_cc"] +
           fel_additional_lib_deps(),
)

# This build rule (along with :lib_internal) purposefully omits the definitions
# of many declared symbols
cc_library(
    name = "lib",
    copts = fel_copts(),
    visibility = ["//visibility:public"],
    deps = [
        ":lib_internal",
    ],
)

##########################################################################################
######################################### Tests ##########################################
##########################################################################################

cc_library(
    name = "test_main",
    testonly = 1,
    srcs = ["platform/test_main.cc"],
    visibility = ["//visibility:public"],
    deps = ["@com_google_googletest//:gtest"],
)

cc_test(
    name = "lib_tests",
    size = "small",
    srcs = [
        "lib/base/choices_test.cc",
        "lib/base/range_test.cc",
        "lib/containers/pool_test.cc",
    ],
    copts = fel_copts(),
    linkstatic = 1,  # Failed at macos when built with opt
    deps = [
        ":lib",
        ":test_main",
    ],
)

cc_test(
    name = "pool_benchmark",
    srcs = ["lib/containers/pool_benchmark.cc"],
    copts = fel_copts(),
    tags = ["benchmark"],
    deps = [
        ":lib",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)
