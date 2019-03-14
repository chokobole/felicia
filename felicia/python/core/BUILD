package(default_visibility = [
    "//felicia:internal",
])

load(
    "//felicia:felicia.bzl",
    "fel_copts",
)

cc_library(
    name = "type_conversion",
    hdrs = ["lib/type_conversion/protobuf.h"],
    copts = fel_copts(),
    deps = ["@pybind11"],
)
