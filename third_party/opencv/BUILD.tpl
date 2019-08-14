package(default_visibility = ["//visibility:public"])

cc_library(
    name = "opencv",
    linkopts = [
        "-lopencv_core",
        "-lopencv_imgproc",
        "-lopencv_calib3d",
    ],
)
