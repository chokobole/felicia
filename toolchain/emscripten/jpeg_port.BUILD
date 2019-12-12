package(default_visibility = ["//visibility:public"])

cc_import(
    name = "libjpeg",
    static_library = "jpeg_lib/libjpeg.a",
)

cc_library(
    name = "jpeg",
    hdrs = glob([
        "jpeg_include/**",
    ]),
    includes = [
        "jpeg_include",
    ],
    deps = [
        ":libjpeg",
    ],
)
