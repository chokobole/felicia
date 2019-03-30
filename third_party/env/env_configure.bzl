def _env_autoconf_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/env:BUILD"), "BUILD")
    felicia_root = repository_ctx.os.environ["PWD"]

    repository_ctx.template(
        "env.bzl",
        Label("//third_party/env:env.bzl.tpl"),
        {
            "%{FELICIA_ROOT}": "\"%s\"" % felicia_root,
        },
    )

env_configure = repository_rule(
    implementation = _env_autoconf_impl,
)
