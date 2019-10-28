package(default_visibility = ["//visibility:public"])

cc_library(
    name = "opencv",
    defines = [
        "HAS_OPENCV",
    ],
    linkopts = [
        "-lopencv_core",
        "-lopencv_imgproc",
        "-lopencv_calib3d",
    ],
)
