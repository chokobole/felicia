licenses(["notice"])  # BSD 3-Clause

load(
    "//third_party/chromium:chromium.bzl",
    "chromium_c_library",
)

package(default_visibility = ["//third_party/chromium:internal"])

chromium_c_library(
    name = "modp_b64",
    srcs = [
        "modp_b64.cc",
    ],
    hdrs = [
        "modp_b64.h",
        "modp_b64_data.h",
    ],
    deps = [
        "//third_party/chromium:build_hdrs",
    ],
)
