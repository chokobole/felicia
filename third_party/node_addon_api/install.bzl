load("@local_config_python//:py.bzl", "PYTHON2_BIN")
load("@env//:env.bzl", "TRAVIS")

def _npm_install_node_addon_api_impl(repository_ctx):
    repository_ctx.symlink(Label("//third_party/node_addon_api:BUILD.bazel"), "BUILD.bazel")
    repository_ctx.symlink(Label("//third_party/node_addon_api:package.json"), "package.json")
    repository_ctx.symlink(Label("//third_party/node_addon_api:package-lock.json"), "package-lock.json")
    repository_ctx.symlink(Label("//third_party/node_addon_api:binding.gyp"), "binding.gyp")
    cmd = ["npm", "install"]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        fail("Failed to npm install.", result.stdout)

    cmd = ["node-gyp", "configure", "--python", PYTHON2_BIN, "--devdir", "."]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        fail("Failed to node-gyp configure.", result.stdout)

    cmd = ["node", "--version"]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        fail("Failed to node --version", result.stdout)

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
