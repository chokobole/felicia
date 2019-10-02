package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ros",
    hdrs = [":ros_include"],
    includes = ["ros_include"],
    linkopts = [
        %{ROS_LIBS}
    ]
)

%{ROS_INCLUDE_GENRULE}