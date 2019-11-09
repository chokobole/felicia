load("//third_party/chromium:chromium.bzl", "chromium_cc_library")

chromium_cc_library(
    name = "chromium",
    includes = ["."],
    visibility = ["//third_party/chromium:internal"],
    deps = [
        "//third_party/chromium/base",
        "//third_party/chromium/device",
        "//third_party/chromium/net",
    ],
)