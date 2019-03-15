licenses(["notice"])  # BSD 3-Clause

package(default_visibility = ["//third_party/chromium/build:internal"])

load(
    "//third_party/chromium/build:chromium.bzl",
    "chromium_copts",
    "chromium_hdrs",
    "chromium_srcs",
    "chromium_test_copts",
    "chromium_test_srcs",
)

cc_library(
    name = "url",
    srcs = chromium_srcs(),
    hdrs = chromium_hdrs(),
    copts = chromium_copts(),
    deps = [
        "//third_party/chromium/base",
    ],
)

cc_test(
    name = "test",
    size = "small",
    srcs = chromium_test_srcs(),
    copts = chromium_test_copts(),
    tags = [
        "chromium",
        "third_party",
    ],
    deps = [
        ":url",
        "//felicia/core:test_main",
    ],
)
