load("//bazel:felicia_proto.bzl", "fel_proto_library")

package_group(
    name = "internal",
    packages = [
        "//felicia/examples/learn/message_communication/...",
    ],
)

fel_proto_library(
    name = "message_spec_proto",
    srcs = ["message_spec.proto"],
    visibility = [":internal"],
)
