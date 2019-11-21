package(default_visibility = ["//visibility:public"])

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
    name = "xmlrpcpp",
    interface_library = "ros_lib/xmlrpcpp.lib",
    system_provided = 1,
)

cc_library(
    name = "ros",
    hdrs = glob([
        "ros_include/**",
        "rosdeps_include/**",
    ]),
    includes = [
        "ros_include",
        "rosdeps_include",
        "rosdeps_include/boost-1_66",
    ],
    defines = [
        "HAS_ROS",
    ],
    deps = [
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
    ],
)
