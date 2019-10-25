package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ros",
    hdrs = glob([
        "ros_include/**"
    ]),
    includes = ["ros_include"],
    linkopts = [
        %{ROS_LIBS}
    ]
)

cc_library(
    name = "actionlib_msgs",
    linkopts = [
        %{ACTIONLIB_MSGS_LIBS}
    ]
)

cc_library(
    name = "controller_manager_msgs",
    linkopts = [
        %{CONTROLLER_MANAGER_MSGS_LIBS}
    ]
)

cc_library(
    name = "control_msgs",
    linkopts = [
        %{CONTROL_MSGS_LIBS}
    ]
)

cc_library(
    name = "diagnostic_msgs",
    linkopts = [
        %{DIAGNOSTIC_MSGS_LIBS}
    ]
)

cc_library(
    name = "gazebo_msgs",
    linkopts = [
        %{GAZEBO_MSGS_LIBS}
    ]
)

cc_library(
    name = "geometry_msgs",
    linkopts = [
        %{GEOMETRY_MSGS_LIBS}
    ]
)

cc_library(
    name = "map_msgs",
    linkopts = [
        %{MAP_MSGS_LIBS}
    ]
)

cc_library(
    name = "nav_msgs",
    linkopts = [
        %{NAV_MSGS_LIBS}
    ]
)

cc_library(
    name = "pcl_msgs",
    linkopts = [
        %{PCL_MSGS_LIBS}
    ]
)

cc_library(
    name = "rosgraph_msgs",
    linkopts = [
        %{ROSGRAPH_MSGS_LIBS}
    ]
)

cc_library(
    name = "sensor_msgs",
    linkopts = [
        %{SENSOR_MSGS_LIBS}
    ]
)

cc_library(
    name = "shape_msgs",
    linkopts = [
        %{SHAPE_MSGS_LIBS}
    ]
)

cc_library(
    name = "smach_msgs",
    linkopts = [
        %{SMACH_MSGS_LIBS}
    ]
)

cc_library(
    name = "std_msgs",
    linkopts = [
        %{STD_MSGS_LIBS}
    ]
)

cc_library(
    name = "stereo_msgs",
    linkopts = [
        %{STEREO_MSGS_LIBS}
    ]
)

cc_library(
    name = "tf2_geometry_msgs",
    linkopts = [
        %{TF2_GEOMETRY_MSGS_LIBS}
    ]
)

cc_library(
    name = "tf2_msgs",
    linkopts = [
        %{TF2_MSGS_LIBS}
    ]
)

cc_library(
    name = "trajectory_msgs",
    linkopts = [
        %{TRAJECTORY_MSGS_LIBS}
    ]
)

cc_library(
    name = "visualization_msgs",
    linkopts = [
        %{VISUALIZATION_MSGS_LIBS}
    ]
)
