def _get_env(repository_ctx, name, default_value = False):
    if name in repository_ctx.os.environ:
        return repository_ctx.os.environ[name]
    return default_value

def _env_autoconf_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/env:BUILD"), "BUILD")
    felicia_root = repository_ctx.os.environ["PWD"]
    travis = _get_env(repository_ctx, "TRAVIS")
    ros_distro = _get_env(repository_ctx, "ROS_DISTRO")

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
