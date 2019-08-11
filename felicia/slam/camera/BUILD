load("//bazel:felicia.bzl", "if_has_opencv")
load("//bazel:felicia_cc.bzl", "fel_cc_library")
load("//bazel:felicia_proto.bzl", "fel_proto_library")

package(default_visibility = ["//felicia:internal"])

fel_proto_library(
    name = "camera_proto",
    srcs = [
        "camera_model_message.proto",
    ],
    default_header = True,
    protodeps = ["//felicia/core/protobuf:protos_all_proto"],
    visibility = ["//visibility:public"],
)

fel_cc_library(
    name = "camera",
    hdrs = if_has_opencv([
        "camera_model.h",
        "stereo_camera_model.h",
    ]),
    srcs = if_has_opencv([
        "camera_model.cc",
        "stereo_camera_model.cc",
    ]),
    deps = [
        "//felicia/core/lib",
    ],
)
