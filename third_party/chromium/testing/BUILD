licenses(["notice"])  # BSD 3-Clause

load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
)

package(default_visibility = ["//third_party/chromium:internal"])

chromium_cc_library(
    name = "testing",
    hdrs = [
        "gmock/include/gmock/gmock.h",
        "gtest/include/gtest/gtest.h",
        "gtest/include/gtest/gtest_prod.h",
    ],
    deps = ["@com_google_googletest//:gtest"],
)
