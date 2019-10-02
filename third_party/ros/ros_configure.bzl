load(
    "//bazel:felicia_repository.bzl",
    "failed_to_get_cflags_and_libs",
    "is_linux",
    "pkg_config_cflags_and_libs",
    "symlink_genrule_for_dir",
)
load("//bazel:felicia_util.bzl", "red")

_ROS_DISTRO = "ROS_DISTRO"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("ROS Confiugation Error:"), msg))

def get_ros_root(repository_ctx):
    ros_distro = repository_ctx.os.environ.get(_ROS_DISTRO)
    if ros_distro == None:
        _fail("Failed to get ROS root.")
    return "/opt/ros/%s" % ros_distro

def _ros_configure_impl(repository_ctx):
    if not is_linux(repository_ctx):
        _fail("Only implemented on linux.")
    pkgs = ["roscpp_serialization"]
    result = pkg_config_cflags_and_libs(repository_ctx, pkgs)
    if result == None:
        _fail(failed_to_get_cflags_and_libs(pkgs))

    ros_include = get_ros_root(repository_ctx) + "/include"
    ros_include_rule = symlink_genrule_for_dir(
        repository_ctx,
        ros_include,
        "ros_include",
        "ros_include",
    )

    repository_ctx.template(
        "BUILD",
        Label("//third_party/ros:BUILD.tpl"),
        {
            "%{ROS_INCLUDE_GENRULE}": ros_include_rule,
            "%{ROS_LIBS}": ",\n".join(result.libs),
        },
    )

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
