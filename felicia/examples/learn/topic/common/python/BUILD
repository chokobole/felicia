py_library(
    name = "topic_create_flag",
    srcs = ["topic_create_flag.py"],
    visibility = ["//felicia/examples:internal"],
    deps = ["//felicia/examples/learn/common/python:node_create_flag"],
)

py_library(
    name = "import_order_resolver",
    srcs = ["import_order_resolver.py"],
    imports = ["."],
    visibility = ["//felicia/examples:internal"],
)

py_library(
    name = "camera_flag",
    srcs = ["camera_flag.py"],
    visibility = ["//felicia/examples:internal"],
    deps = [":topic_create_flag"],
)
