def _env_autoconf_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/env:BUILD"), "BUILD")
    felicia_root = repository_ctx.os.environ["PWD"]
    travis = "TRAVIS" in repository_ctx.os.environ and repository_ctx.os.environ["TRAVIS"]

    repository_ctx.template(
        "env.bzl",
        Label("//third_party/env:env.bzl.tpl"),
        {
            "%{FELICIA_ROOT}": "\"%s\"" % felicia_root,
            "%{TRAVIS}": "\"%s\"" % travis,
        },
    )

env_configure = repository_rule(
    implementation = _env_autoconf_impl,
)
