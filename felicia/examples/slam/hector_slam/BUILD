load("//bazel:felicia_cc.bzl", "fel_cc_library")

package(default_visibility = ["//felicia/examples:internal"])

fel_cc_library(
    name = "hector_slam_node",
    srcs = [
        "hector_slam.cc",
        "hector_slam_node.cc",
        "log_odds_occupancy_grid_map.cc",
        "multi_resolution_grid_map.cc",
        "point_container.cc",
    ],
    hdrs = [
        "hector_slam.h",
        "hector_slam_node.h",
        "log_odds_occupancy_grid_map.h",
        "multi_resolution_grid_map.h",
        "point_container.h",
        "scan_matcher.h",
    ],
    is_external = False,
    deps = [
        "//felicia/core/communication",
        "//felicia/examples/slam:slam_node_create_flag",
        "//felicia/slam",
    ],
)
