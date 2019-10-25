def _env_autoconf_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/env:BUILD"), "BUILD")
    felicia_root = repository_ctx.os.environ["PWD"]
    travis = "TRAVIS" in repository_ctx.os.environ and repository_ctx.os.environ["TRAVIS"]
    ros_distro = "ROS_DISTRO" in repository_ctx.os.environ and repository_ctx.os.environ["ROS_DISTRO"]

    repository_ctx.template(
        "env.bzl",
        Label("//third_party/env:env.bzl.tpl"),
        {
            "%{FELICIA_ROOT}": "\"%s\"" % felicia_root,
            "%{TRAVIS}": "\"%s\"" % travis,
            "%{ROS_DISTRO}": "\"%s\"" % ros_distro,
        },
    )

env_configure = repository_rule(
    implementation = _env_autoconf_impl,
)
