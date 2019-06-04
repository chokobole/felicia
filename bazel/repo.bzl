load("@bazel_tools//tools/cpp:lib_cc_configure.bzl", "get_cpu_value")
load("@bazel_tools//tools/build_defs/repo:utils.bzl", "patch", "update_attrs", "workspace_and_buildfile")

def find_bash(repository_ctx):
    return repository_ctx.os.environ["BAZEL_SH"] if "BAZEL_SH" in repository_ctx.os.environ else repository_ctx.which("bash")

_http_archive_attrs = {
    "url_fmt": attr.string(mandatory = True),
    "fmt_dict": attr.string_list_dict(mandatory = True),
    "sha256": attr.string(),
    "strip_prefix_dict": attr.string_dict(),
    "type": attr.string(),
    "build_file": attr.label(allow_single_file = True),
    "build_file_content": attr.string(),
    "patches": attr.label_list(default = []),
    "patch_tool": attr.string(default = "patch"),
    "patch_args": attr.string_list(default = ["-p0"]),
    "patch_cmds": attr.string_list(default = []),
    "workspace_file": attr.label(allow_single_file = True),
    "workspace_file_content": attr.string(),
}

# Please refer to https://github.com/bazelbuild/bazel/blob/0.24.0/tools/build_defs/repo/http.bzl
def _http_archive_per_os_impl(repository_ctx):
    if repository_ctx.attr.build_file and repository_ctx.attr.build_file_content:
        fail("Only one of build_file and build_file_content can be provided.")

    cpu_value = get_cpu_value(repository_ctx)
    if cpu_value not in repository_ctx.attr.fmt_dict:
        fail("You should add fmt_dict for %s" % cpu_value)
    url = repository_ctx.attr.url_fmt % tuple(repository_ctx.attr.fmt_dict[cpu_value])

    if cpu_value not in repository_ctx.attr.strip_prefix_dict:
        fail("You should add strip_prefix_dict for %s" % cpu_value)
    strip_prefix = repository_ctx.attr.strip_prefix_dict[cpu_value]

    download_info = repository_ctx.download_and_extract(
        url,
        "",
        repository_ctx.attr.sha256,
        repository_ctx.attr.type,
        strip_prefix,
    )
    patch(repository_ctx)
    workspace_and_buildfile(repository_ctx)

    return update_attrs(repository_ctx.attr, _http_archive_attrs.keys(), {"sha256": download_info.sha256})

http_archive_per_os = repository_rule(
    implementation = _http_archive_per_os_impl,
    attrs = _http_archive_attrs,
)
