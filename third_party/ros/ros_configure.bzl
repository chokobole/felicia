load(
    "//bazel:repo.bzl",
    "failed_to_get_cflags_and_libs",
    "pkg_config_cflags_and_libs",
    "symlink_dir",
)
load(
    "//bazel:platform.bzl",
    "is_linux",
    "is_windows",
    "is_x64",
)
load("//bazel:text_style.bzl", "red")

_ROS_DISTRO = "ROS_DISTRO"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("ROS Confiugation Error:"), msg))

def get_ros_distro(repository_ctx):
    ros_distro = repository_ctx.os.environ.get(_ROS_DISTRO)
    if ros_distro == None:
        _fail("Failed to get ROS_DISTRO.")
    return ros_distro

def _pkg_config_cflags_and_libs_or_die(repository_ctx, pkgs):
    result = pkg_config_cflags_and_libs(repository_ctx, pkgs)
    if result == None:
        _fail(failed_to_get_cflags_and_libs(pkgs))
    return result

def _ros_configure_impl_linux(repository_ctx):
    ros = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["roscpp"])
    actionlib_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["actionlib_msgs"])
    controller_manager_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["controller_manager_msgs"])
    control_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["control_msgs"])
    diagnostic_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["diagnostic_msgs"])
    gazebo_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["gazebo_msgs"])
    geometry_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["geometry_msgs"])
    map_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["map_msgs"])
    nav_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["nav_msgs"])
    pcl_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["pcl_msgs"])
    rosgraph_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["rosgraph_msgs"])
    sensor_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["sensor_msgs"])
    shape_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["shape_msgs"])
    smach_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["smach_msgs"])
    std_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["std_msgs"])
    stereo_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["stereo_msgs"])
    tf2_geometry_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["tf2_geometry_msgs"])
    tf2_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["tf2_msgs"])
    trajectory_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["trajectory_msgs"])
    visualization_msgs = _pkg_config_cflags_and_libs_or_die(repository_ctx, ["visualization_msgs"])

    ros_root = "/opt/ros/%s" % get_ros_distro(repository_ctx)
    ros_include = ros_root + "/include"
    symlink_dir(
        repository_ctx,
        ros_include,
        "ros_include",
    )

    repository_ctx.template(
        "BUILD",
        Label("//third_party/ros:BUILD.tpl"),
        {
            "%{ROS_LIBS}": ",\n".join(ros.libs),
            "%{ACTIONLIB_MSGS_LIBS}": ",\n".join(actionlib_msgs.libs),
            "%{CONTROLLER_MANAGER_MSGS_LIBS}": ",\n".join(controller_manager_msgs.libs),
            "%{CONTROL_MSGS_LIBS}": ",\n".join(control_msgs.libs),
            "%{DIAGNOSTIC_MSGS_LIBS}": ",\n".join(diagnostic_msgs.libs),
            "%{GAZEBO_MSGS_LIBS}": ",\n".join(gazebo_msgs.libs),
            "%{GEOMETRY_MSGS_LIBS}": ",\n".join(geometry_msgs.libs),
            "%{MAP_MSGS_LIBS}": ",\n".join(map_msgs.libs),
            "%{NAV_MSGS_LIBS}": ",\n".join(nav_msgs.libs),
            "%{PCL_MSGS_LIBS}": ",\n".join(pcl_msgs.libs),
            "%{ROSGRAPH_MSGS_LIBS}": ",\n".join(rosgraph_msgs.libs),
            "%{SENSOR_MSGS_LIBS}": ",\n".join(sensor_msgs.libs),
            "%{SHAPE_MSGS_LIBS}": ",\n".join(shape_msgs.libs),
            "%{SMACH_MSGS_LIBS}": ",\n".join(smach_msgs.libs),
            "%{STD_MSGS_LIBS}": ",\n".join(std_msgs.libs),
            "%{STEREO_MSGS_LIBS}": ",\n".join(stereo_msgs.libs),
            "%{TF2_GEOMETRY_MSGS_LIBS}": ",\n".join(tf2_geometry_msgs.libs),
            "%{TF2_MSGS_LIBS}": ",\n".join(tf2_msgs.libs),
            "%{TRAJECTORY_MSGS_LIBS}": ",\n".join(trajectory_msgs.libs),
            "%{VISUALIZATION_MSGS_LIBS}": ",\n".join(visualization_msgs.libs),
        },
    )

def _ros_configure_impl_windows(repository_ctx):
    ros_root = "C:\\opt\\ros\\%s" % get_ros_distro(repository_ctx)
    rosdeps_root = "C:\\opt\\rosdeps"
    if is_x64(repository_ctx):
        ros_root += "\\x64"
        rosdeps_root += "\\x64"
    else:
        ros_root += "\\x86"
        rosdeps_root += "\\x86"
    ros_include = ros_root + "/include"
    symlink_dir(repository_ctx, ros_root + "\\include", "ros_include")
    symlink_dir(repository_ctx, ros_root + "\\lib", "ros_lib", src_files = [
        "actionlib.lib",
        "cpp_common.lib",
        "message_filters.lib",
        "orocos-kdl.lib",
        "rosconsole.lib",
        "rosconsole_backend_interface.lib",
        "rosconsole_log4cxx.lib",
        "roscpp.lib",
        "roscpp_serialization.lib",
        "rostime.lib",
        "tf2_ros.lib",
        "xmlrpcpp.lib",
    ])

    symlink_dir(repository_ctx, rosdeps_root + "\\include", "rosdeps_include", src_files = [
        "boost-1_66",
        "console_bridge",
        "console_bridge_export.h",
        "log4cxx",
    ])
    symlink_dir(repository_ctx, rosdeps_root + "\\lib", "rosdeps_lib", src_files = [
        "boost_atomic-vc141-mt-x64-1_66.lib",
        "boost_chrono-vc141-mt-x64-1_66.lib",
        "boost_date_time-vc141-mt-x64-1_66.lib",
        "boost_filesystem-vc141-mt-x64-1_66.lib",
        "boost_regex-vc141-mt-x64-1_66.lib",
        "boost_system-vc141-mt-x64-1_66.lib",
        "boost_thread-vc141-mt-x64-1_66.lib",
        "console_bridge.lib",
        "log4cxx.lib",
    ])

    repository_ctx.symlink(Label("//third_party/ros:ros_windows.BUILD"), "BUILD")

def _ros_configure_impl(repository_ctx):
    if is_linux(repository_ctx):
        _ros_configure_impl_linux(repository_ctx)
    elif is_windows(repository_ctx):
        _ros_configure_impl_windows(repository_ctx)
    else:
        _fail("Not supported platform.")

ros_configure = repository_rule(
    implementation = _ros_configure_impl,
)
"""Detects and configure the ros header and library

Add the following to your WORKSPACE FILE:

```python
ros_configure(name = "local_config_ros")
```
Args:
    name: A unique name for the workspace rule.
"""
