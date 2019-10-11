package(default_visibility = ["//visibility:private"])

py_binary(
    name = "human_pose_estimation_from_camera",
    srcs = [
        "human_pose_estimation_from_camera.py",
        "openpose.py",
    ],
    main = "human_pose_estimation_from_camera.py",
    deps = [
        "//felicia/examples/learn/topic/common/python:import_order_resolver",
        "//felicia/python:felicia_py",
    ],
)
