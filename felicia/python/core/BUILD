load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

package(default_visibility = ["//felicia:internal"])

cc_library(
    name = "type_conversion",
    hdrs = ["lib/type_conversion/protobuf.h"],
    copts = fel_copts(),
    deps = ["@pybind11"],
)
