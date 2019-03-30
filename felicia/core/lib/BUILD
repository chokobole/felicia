load("//bazel:felicia.bzl", "fel_shared_library")
load(
    "//bazel:felicia_cc.bzl",
    "fel_cc_library",
    "fel_cc_test",
)

package(default_visibility = ["//felicia:internal"])

fel_cc_library(
    name = "lib",
    srcs = [
        "base/task_runner_interface.cc",
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
    deps = [
        "//felicia/core/protobuf:protos_all_cc",
        "//third_party/chromium/base",
        "//third_party/chromium/net",
        "@com_google_googletest//:gtest",
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
    deps = fel_shared_library() + [
        "@com_google_googletest//:gtest_main",
    ],
)

fel_cc_test(
    name = "pool_benchmark",
    srcs = ["containers/pool_benchmark.cc"],
    tags = ["benchmark"],
    deps = fel_shared_library() + [
        "@com_github_google_benchmark//:benchmark_main",
    ],
)
