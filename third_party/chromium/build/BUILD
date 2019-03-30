licenses(["notice"])  # BSD 3-Clause

load(
    "//third_party/chromium:chromium.bzl",
    "chromium_cc_library",
)
load("//third_party/chromium/build:timestamp.bzl", "build_timestamp")

package(default_visibility = ["//third_party/chromium:internal"])

exports_files([
    "write_buildflag_header.py",
    "write_build_date_header.py",
])

chromium_cc_library(
    name = "build",
    hdrs = [
        "build_config.h",
        "buildflag.h",
    ],
)

build_timestamp(
    name = "timestamp",
    dep = "//third_party/chromium/build/util:lastchange",
)
