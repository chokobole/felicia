load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
    "chromium_cc_shared_library",
)

chromium_cc_library(
    name = "chromium",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/chromium/base",
        "//third_party/chromium/device",
        "//third_party/chromium/net",
    ],
)

chromium_cc_shared_library(
    name = "chromium",
    srcs = ["@com_github_chokobole_felicia//felicia:empty.cc"],
    deps = [
        "//third_party/chromium/base",
        "//third_party/chromium/device",
        "//third_party/chromium/net",
    ],
)

chromium_cc_library(
    name = "chromium_hdrs",
    includes = ["."],
    visibility = ["//visibility:public"],
    deps = [
        "//third_party/chromium:base_hdrs",
        "//third_party/chromium:device_hdrs",
        "//third_party/chromium:net_hdrs",
    ],
)
