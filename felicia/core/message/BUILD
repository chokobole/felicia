load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "message",
    srcs = [
        "dynamic_protobuf_message.cc",
        "header.cc",
        "message_io.cc",
        "protobuf_loader.cc",
        "protobuf_util.cc",
    ],
    hdrs = [
        "dynamic_protobuf_message.h",
        "header.h",
        "message_filter.h",
        "message_io.h",
        "message_io_error_list.h",
        "protobuf_loader.h",
        "protobuf_util.h",
    ],
    deps = [
        "//felicia/core/lib",
        "//felicia/core/util:command_line_interface",
    ],
)

fel_cc_library(
    name = "message_test_util",
    testonly = True,
    hdrs = [
        "test/a_message.h",
    ],
    deps = [
        ":message",
    ],
)

fel_cc_test(
    name = "message_tests",
    size = "small",
    srcs = [
        "message_filter_test.cc",
    ],
    deps = fel_shared_library() + [
        ":message_test_util",
        "@com_google_googletest//:gtest_main",
    ],
)
