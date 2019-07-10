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
load(
    "//third_party/chromium/device:device.bzl",
    "device_copts",
    "device_linkopts",
)

chromium_cc_library(
    name = "device",
    srcs = chromium_platform_srcs(),
    hdrs = chromium_platform_hdrs(),
    copts = device_copts(),
    linkopts = device_linkopts(),
    deps = ["//third_party/chromium/base"],
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
        ":device",
        "@com_google_googletest//:gtest_main",
    ],
)
