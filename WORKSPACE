workspace(name = "com_github_chokobole_felicia")

load("//bazel:felicia_deps.bzl", "felicia_deps")

felicia_deps()

load("//third_party/node_addon_api:install.bzl", "npm_install_node_addon_api")

npm_install_node_addon_api(name = "node_addon_api")

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

load("@io_bazel_rules_go//go:def.bzl", "go_register_toolchains", "go_rules_dependencies")
load("@com_github_bazelbuild_buildtools//buildifier:deps.bzl", "buildifier_dependencies")

go_rules_dependencies()

go_register_toolchains()

buildifier_dependencies()

load("@rules_pkg//:deps.bzl", "rules_pkg_dependencies")

rules_pkg_dependencies()
