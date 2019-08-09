package(default_visibility = ["//visibility:public"])

cc_library(
    name = "opencv",
    linkopts = ["-lopencv_core"],
)
