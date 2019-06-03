load("//bazel:felicia_wasm.bzl", "fel_wasm_binary")

fel_wasm_binary(
    name = "felicia_wasm.js",
    srcs = ["felicia_wasm.cc"],
    copts = [
        "--bind",
    ],
    linkopts = [
        "--bind",
        "-s ENVIRONMENT='worker'",
        "-s ALLOW_MEMORY_GROWTH=1",
    ],
    deps = [
        "@libyuv",
    ]
)
