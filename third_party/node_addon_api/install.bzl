load("@local_config_python//:py.bzl", "PYTHON2_BIN")
load(
    "//bazel:repo.bzl",
    "failed_to_find_bash_bin_path",
    "get_bash_bin_path",
)
load("//bazel:text_style.bzl", "red")

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("Node Addon Install Error:"), msg))

def _npm_install_node_addon_api_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/node_addon_api:BUILD.bazel"), "BUILD.bazel")
    repository_ctx.symlink(Label("//third_party/node_addon_api:package.json"), "package.json")
    repository_ctx.symlink(Label("//third_party/node_addon_api:package-lock.json"), "package-lock.json")
    repository_ctx.symlink(Label("//third_party/node_addon_api:binding.gyp"), "binding.gyp")

    bash = get_bash_bin_path(repository_ctx)
    if bash == None:
        _fail(failed_to_find_bash_bin_path(repository_ctx))
    cmd = [bash, "-c", "npm install"]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        _fail("Failed to npm install: %s." % result.stdout)

    cmd = [bash, "-c", "npx node-gyp configure --python %s --devdir ." % PYTHON2_BIN]

    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        _fail("Failed to node-gyp configure: %s." % result.stdout)

    cmd = [bash, "-c", "node --version"]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        _fail("Failed to node --version: %s." % result.stdout)

    version = result.stdout.strip()
    if version.startswith("v"):
        version = version[1:]

    repository_ctx.template(
        "node_addon_api.bzl",
        Label("//third_party/node_addon_api:node_addon_api.bzl.tpl"),
        {
            "%{NODE_VERSION}": "\"%s\"" % (version),
        },
    )

npm_install_node_addon_api = repository_rule(
    implementation = _npm_install_node_addon_api_impl,
)
