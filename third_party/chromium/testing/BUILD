licenses(["notice"])  # BSD 3-Clause

load(
    "//bazel:felicia.bzl",
    "if_mac_or_ios",
)
load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
    "chromium_objc_library",
)

package(default_visibility = ["//third_party/chromium:internal"])

TESTING_HDRS = [
    "gmock/include/gmock/gmock.h",
    "gmock/include/gmock/gmock-actions.h",
    "gmock/include/gmock/gmock-generated-function-mockers.h",
    "gmock/include/gmock/gmock-matchers.h",
    "gtest/include/gtest/gtest.h",
    "gtest/include/gtest/gtest_prod.h",
    "gtest/include/gtest/gtest-death-test.h",
    "gtest/include/gtest/gtest-message.h",
    "gtest/include/gtest/gtest-param-test.h",
    "gtest/include/gtest/gtest-spi.h",
    "platform_test.h",
    "multiprocess_func_list.h",
]

chromium_cc_library(
    name = "testing",
    srcs = [
        "multiprocess_func_list.cc",
    ],
    hdrs = TESTING_HDRS,
    deps = if_mac_or_ios([
        ":testing_mac",
    ]) + ["@com_google_googletest//:gtest"],
)

chromium_objc_library(
    name = "testing_mac",
    srcs = ["platform_test_mac.mm"],
    hdrs = TESTING_HDRS,
    deps = ["@com_google_googletest//:gtest"],
)
