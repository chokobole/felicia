load("//bazel:felicia.bzl", "if_has_orb_slam2")
load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia/examples:internal"])

cc_import(
    name = "lib_orb_slam2",
    interface_library = select({
        "//felicia:windows": "not-exsiting.lib",
        "//conditions:default": "ORB_SLAM2/lib/libORB_SLAM2.so",
    }),
    shared_library = "ORB_SLAM2/lib/libORB_SLAM2.so",
)

cc_library(
    name = "@orb_slam2",
    hdrs = if_has_orb_slam2(glob([
        "ORB_SLAM2/include/*.h",
        "ORB_SLAM2/Thirdparty/DBoW2/**/*.h",
        "ORB_SLAM2/Thirdparty/g2o/**/*.h",
        "ORB_SLAM2/Thirdparty/g2o/**/*.hpp",
    ])),
    includes = if_has_orb_slam2([
        "ORB_SLAM2",
        "ORB_SLAM2/include",
        "ORB_SLAM2/Thirdparty/DBoW2/DBoW2",
        "ORB_SLAM2/Thirdparty/DBoW2/DUtils",
        "ORB_SLAM2/Thirdparty/g2o",
        "ORB_SLAM2/Thirdparty/g2o/g2o/core",
        "ORB_SLAM2/Thirdparty/g2o/g2o/solvers",
        "ORB_SLAM2/Thirdparty/g2o/g2o/stuff",
        "ORB_SLAM2/Thirdparty/g2o/g2o/types",
    ]),
    linkopts = if_has_orb_slam2([
        "-L/usr/local/lib",
        "-lpangolin",
        "-lGL",
        "-lGLEW",
    ]),
    deps = [":lib_orb_slam2"],
)

fel_cc_library(
    name = "orb_slam2_node",
    srcs = if_has_orb_slam2([
        "orb_slam2_node.cc",
    ]),
    hdrs = if_has_orb_slam2([
        "orb_slam2_node.h",
    ]),
    is_external = False,
    deps = [
        "//felicia/core/communication",
        "//felicia/slam",
    ] + if_has_orb_slam2([
        ":@orb_slam2",
        "//felicia/examples/slam:slam_node_create_flag",
    ]),
)
