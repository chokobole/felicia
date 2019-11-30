# Copyright (c) 2019 The Felicia Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication",
    srcs = [
        "dynamic_publisher.cc",
        "dynamic_subscriber.cc",
        "serialized_message_publisher.cc",
        "serialized_message_subscriber.cc",
    ],
    hdrs = [
        "dynamic_publisher.h",
        "dynamic_subscriber.h",
        "publisher.h",
        "register_state.h",
        "serialized_message_publisher.h",
        "serialized_message_subscriber.h",
        "service_client.h",
        "service_server.h",
        "settings.h",
        "subscriber.h",
        "subscriber_state.h",
    ],
    deps = [
        "//felicia/core/channel",
        "//felicia/core/master:master_proxy",
        "//felicia/core/rpc",
    ],
)

fel_cc_test(
    name = "communication_unittests",
    size = "small",
    srcs = [
        "pubsub_unittest.cc",
        "service_unittest.cc",
    ],
    deps = [
        ":communication",
        "//felicia/core/lib:lib_test_util",
        "//felicia/core/message:message_test_util",
        "//felicia/core/util:timestamp",
        "@com_google_googletest//:gtest_main",
    ],
)
