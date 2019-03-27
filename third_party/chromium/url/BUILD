licenses(["notice"])  # BSD 3-Clause

package(default_visibility = ["//third_party/chromium:internal"])

load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
    "chromium_cc_test",
    "chromium_platform_hdrs",
    "chromium_platform_srcs",
    "chromium_platform_test_srcs",
)

chromium_cc_library(
    name = "url",
    srcs = chromium_platform_srcs(),
    hdrs = chromium_platform_hdrs(),
    deps = [
        "//third_party/chromium/base",
    ],
)

chromium_cc_test(
    name = "test",
    size = "small",
    srcs = chromium_platform_test_srcs(),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":url",
        "@com_google_googletest//:gtest_main",
    ],
)
