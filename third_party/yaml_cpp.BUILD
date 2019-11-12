# Description:
#   yaml-cpp is a A YAML parser and emitter in C++

licenses(["notice"])  # MIT

exports_files(["LICENSE"])

cc_library(
    name = "yaml_cpp",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    hdrs = glob([
        "include/yaml-cpp/**/*.h",
    ]),
    includes = [
        "include",
        "src",
    ],
    visibility = ["//visibility:public"],
)
