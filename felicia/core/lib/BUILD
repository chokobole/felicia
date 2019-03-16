load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "error_codes_proto",
    srcs = ["error/error_codes.proto"],
    visibility = ["//felicia:internal"],
)

fel_cc_library(
    name = "lib",
    srcs = [
        "error/status.cc",
        "error/statusor.cc",
        "net/net_util.cc",
        "strings/str_util.cc",
    ],
    hdrs = [
        "base/choices.h",
        "base/export.h",
        "base/range.h",
        "base/task_runner_interface.h",
        "base/template_util.h",
        "containers/pool.h",
        "error/errors.h",
        "error/status.h",
        "error/statusor.h",
        "error/statusor_internals.h",
        "net/net_util.h",
        "strings/str_util.h",
    ],
    linkopts = select({
        "//felicia:windows": [],
        "//conditions:default": [
            "-lpthread",
        ],
    }),
    deps = [
        ":error_codes_proto_cc",
        "//third_party/chromium",
        "@com_google_googletest//:gtest",
        "@com_google_protobuf//:protobuf",
    ],
)

fel_cc_test(
    name = "lib_tests",
    size = "small",
    srcs = [
        "base/choices_test.cc",
        "base/range_test.cc",
        "containers/pool_test.cc",
    ],
    linkstatic = 1,  # Failed at macos when built with opt
    deps = [
        ":lib",
        "@com_google_googletest//:gtest_main",
    ],
)

fel_cc_test(
    name = "pool_benchmark",
    srcs = ["containers/pool_benchmark.cc"],
    tags = ["benchmark"],
    deps = [
        ":lib",
        "@com_github_google_benchmark//:benchmark_main",
    ],
)
