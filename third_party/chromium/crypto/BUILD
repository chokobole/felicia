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
    "//third_party/chromium/crypto:crypto.bzl",
    "crypto_copts",
    "crypto_defines",
    "crypto_includes",
    "crypto_linkopts",
)

CRYPTO_HDRS = chromium_platform_hdrs()

filegroup(
    name = "crypto_hdrs",
    srcs = CRYPTO_HDRS,
)

chromium_cc_library(
    name = "crypto",
    srcs = chromium_platform_srcs(),
    hdrs = CRYPTO_HDRS,
    copts = crypto_copts(),
    defines = crypto_defines(),
    includes = crypto_includes(),
    linkopts = crypto_linkopts(),
    deps = [
        "//third_party/chromium/base",
        "@boringssl//:ssl",
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
        ":device",
        "@com_google_googletest//:gtest_main",
    ],
)
