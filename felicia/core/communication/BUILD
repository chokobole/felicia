load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "communication",
    srcs = [
        "dynamic_publisher.cc",
        "dynamic_subscriber.cc",
        "ros_header.cc",
        "serialized_message_publisher.cc",
        "serialized_message_subscriber.cc",
    ],
    hdrs = [
        "dynamic_publisher.h",
        "dynamic_subscriber.h",
        "publisher.h",
        "register_state.h",
        "ros_header.h",
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
