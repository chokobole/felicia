package(default_visibility = ["//visibility:private"])

py_binary(
    name = "object_detection_from_camera",
    srcs = [
        "object_detection_from_camera.py",
        "tf_object_detection.py",
    ],
    main = "object_detection_from_camera.py",
    deps = [
        "//felicia/examples/learn/message_communication/common/python:import_order_resolver",
        "//felicia/python:felicia_py",
    ],
)
