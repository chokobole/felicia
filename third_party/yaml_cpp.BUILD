# Description:
#   yaml-cpp is a A YAML parser and emitter in C++

licenses(["notice"])  # MIT

exports_files(["LICENSE"])

cc_library(
    name = "yaml-cpp",
    hdrs = glob([
        "include/yaml-cpp/**/*.h",
    ]),
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    includes = ["src", "include"],
    visibility = ["//visibility:public"],
)
