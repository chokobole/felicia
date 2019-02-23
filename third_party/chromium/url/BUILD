
package(default_visibility = ["//third_party/chromium/build:internal"])
licenses(["notice"])

load(
    "//third_party/chromium/build:chromium.bzl",
    "chromium_hdrs",
    "chromium_srcs",
    "chromium_test_srcs",
    "chromium_copts",
    "chromium_test_copts"
)

cc_library(
    name = "url",
    hdrs = chromium_hdrs(),
    srcs = chromium_srcs(),
    copts = chromium_copts(),
    deps = [
        "//third_party/chromium/base:base"
    ]
)

cc_test(
    name = "test",
    size = "small",
    tags = ["third_party", "chromium"],
    srcs = chromium_test_srcs(),
    copts = chromium_test_copts(),
    deps = [
        ":url",
        "//felicia/core:test_main"
    ]
)