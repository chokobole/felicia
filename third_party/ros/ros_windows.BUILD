package(default_visibility = ["//visibility:public"])

cc_import(
    name = "actionlib",
    interface_library = "ros_lib/actionlib.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_atomic-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_atomic-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_chrono-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_chrono-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_date_time-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_date_time-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_filesystem-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_filesystem-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_regex-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_regex-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_system-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_system-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "boost_thread-vc141-mt-x64-1_66",
    interface_library = "rosdeps_lib/boost_thread-vc141-mt-x64-1_66.lib",
    system_provided = 1,
)

cc_import(
    name = "console_bridge",
    interface_library = "rosdeps_lib/console_bridge.lib",
    system_provided = 1,
)

cc_import(
    name = "cpp_common",
    interface_library = "ros_lib/cpp_common.lib",
    system_provided = 1,
)

cc_import(
    name = "log4cxx",
    interface_library = "rosdeps_lib/log4cxx.lib",
    system_provided = 1,
)

cc_import(
    name = "message_filters",
    interface_library = "ros_lib/message_filters.lib",
    system_provided = 1,
)

cc_import(
    name = "orocos-kdl",
    static_library = "ros_lib/orocos-kdl.lib",
)

cc_import(
    name = "rosconsole",
    interface_library = "ros_lib/rosconsole.lib",
    system_provided = 1,
)

cc_import(
    name = "rosconsole_backend_interface",
    interface_library = "ros_lib/rosconsole_backend_interface.lib",
    system_provided = 1,
)

cc_import(
    name = "rosconsole_log4cxx",
    interface_library = "ros_lib/rosconsole_log4cxx.lib",
    system_provided = 1,
)

cc_import(
    name = "roscpp",
    interface_library = "ros_lib/roscpp.lib",
    system_provided = 1,
)

cc_import(
    name = "roscpp_serialization",
    interface_library = "ros_lib/roscpp_serialization.lib",
    system_provided = 1,
)

cc_import(
    name = "rostime",
    interface_library = "ros_lib/rostime.lib",
    system_provided = 1,
)

cc_import(
    name = "tf2_ros",
    interface_library = "ros_lib/tf2_ros.lib",
    system_provided = 1,
)

cc_import(
    name = "xmlrpcpp",
    interface_library = "ros_lib/xmlrpcpp.lib",
    system_provided = 1,
)

DEPS = [
    ":boost_atomic-vc141-mt-x64-1_66",
    ":boost_chrono-vc141-mt-x64-1_66",
    ":boost_date_time-vc141-mt-x64-1_66",
    ":boost_filesystem-vc141-mt-x64-1_66",
    ":boost_regex-vc141-mt-x64-1_66",
    ":boost_system-vc141-mt-x64-1_66",
    ":boost_thread-vc141-mt-x64-1_66",
    ":console_bridge",
    ":cpp_common",
    ":log4cxx",
    ":rosconsole",
    ":rosconsole_backend_interface",
    ":rosconsole_log4cxx",
    ":roscpp",
    ":roscpp_serialization",
    ":rostime",
    ":xmlrpcpp",
]

cc_library(
    name = "ros",
    hdrs = glob([
        "ros_include/**",
        "rosdeps_include/**",
    ]),
    defines = [
        "HAS_ROS",
    ],
    includes = [
        "ros_include",
        "rosdeps_include",
        "rosdeps_include/boost-1_66",
    ],
    deps = DEPS,
)

cc_library(
    name = "actionlib_msgs",
    deps = DEPS,
)

cc_library(
    name = "controller_manager_msgs",
    deps = DEPS,
)

cc_library(
    name = "control_msgs",
    deps = DEPS,
)

cc_library(
    name = "diagnostic_msgs",
    deps = DEPS,
)

cc_library(
    name = "gazebo_msgs",
    deps = DEPS,
)

cc_library(
    name = "geometry_msgs",
    deps = DEPS,
)

cc_library(
    name = "map_msgs",
    deps = DEPS,
)

cc_library(
    name = "nav_msgs",
    deps = DEPS,
)

cc_library(
    name = "pcl_msgs",
    deps = DEPS,
)

cc_library(
    name = "rosgraph_msgs",
    deps = DEPS,
)

cc_library(
    name = "sensor_msgs",
    deps = DEPS,
)

cc_library(
    name = "shape_msgs",
    deps = DEPS,
)

cc_library(
    name = "smach_msgs",
    deps = DEPS,
)

cc_library(
    name = "std_msgs",
    deps = DEPS,
)

cc_library(
    name = "stereo_msgs",
    deps = DEPS,
)

cc_library(
    name = "tf2_geometry_msgs",
    deps = DEPS + [
        ":actionlib",
        ":message_filters",
        ":oroscos-kdl",
        ":tf2_ros",
    ],
)

cc_library(
    name = "tf2_msgs",
    deps = DEPS,
)

cc_library(
    name = "trajectory_msgs",
    deps = DEPS,
)

cc_library(
    name = "visualization_msgs",
    deps = DEPS,
)
