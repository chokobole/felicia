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
    "device_defines",
    "device_includes",
    "device_linkopts",
)

DEVICE_HDRS = chromium_platform_hdrs()

filegroup(
    name = "device_hdrs",
    srcs = DEVICE_HDRS,
)

chromium_cc_library(
    name = "device",
    srcs = chromium_platform_srcs(),
    hdrs = DEVICE_HDRS,
    copts = device_copts(),
    defines = device_defines(),
    includes = device_includes(),
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
