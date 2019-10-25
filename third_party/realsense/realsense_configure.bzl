load(
    "//bazel:felicia_repository.bzl",
    "is_windows",
    "is_x64",
    "symlink_genrule_for_dir",
)
load(
    "//bazel:felicia_util.bzl",
    "norm_path",
    "red",
)

_REALSENSE_PATH = "REALSENSE_PATH"
_DEFAULT_PATH_ON_WINDOWS = "c:\\Program Files (x86)\\Intel RealSense SDK 2.0"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("Realsense Confiugation Error:"), msg))

def _get_realsense_path(repository_ctx):
    realsense_path = repository_ctx.os.environ.get(_REALSENSE_PATH)
    if realsense_path == None:
        realsense_path = _DEFAULT_PATH_ON_WINDOWS

    if repository_ctx.path(realsense_path).exists:
        return realsense_path
    else:
        _fail("Failed to find the path for realsense, Did you installed sdk " +
              "by default? or please locate the sdk path setting " +
              "enviornment variable \"REALSENSE_PATH\"")

    return None

def _realsense_configure_impl(repository_ctx):
    realsense_include_rule = ""
    realsense_import_lib_genrule = ""
    realsense_import_dll_genrule = ""

    if is_windows(repository_ctx):
        path = _get_realsense_path(repository_ctx)
        realsense_include = path + "/include"
        realsense_include_rule = symlink_genrule_for_dir(
            repository_ctx,
            realsense_include,
            "realsense_include",
            "realsense_include",
        )
        realsense_import_lib_name = "realsense2.lib"
        realsense_import_dll_name = "realsense2.dll"
        if is_x64(repository_ctx):
            realsense_import_lib_src = path + "/lib/x64/" + realsense_import_lib_name
            realsense_import_dll_src = path + "/bin/x64/" + realsense_import_dll_name
        else:
            realsense_import_lib_src = path + "/lib/x86/" + realsense_import_lib_name
            realsense_import_dll_src = path + "/bin/x86/" + realsense_import_dll_name
        realsense_import_lib_genrule = symlink_genrule_for_dir(
            repository_ctx,
            None,
            "",
            "realsense_import_lib",
            [realsense_import_lib_src],
            [realsense_import_lib_name],
        )
        realsense_import_dll_genrule = symlink_genrule_for_dir(
            repository_ctx,
            None,
            "",
            "realsense_import_dll",
            [realsense_import_dll_src],
            [realsense_import_dll_name],
        )

    repository_ctx.template(
        "BUILD",
        Label("//third_party/realsense:BUILD.tpl"),
        {
            "%{REALSENSE_INCLUDE_GENRULE}": realsense_include_rule,
            "%{REALSENSE_IMPORT_LIB_GENRULE}": realsense_import_lib_genrule,
            "%{REALSENSE_IMPORT_DLL_GENRULE}": realsense_import_dll_genrule,
        },
    )

realsense_configure = repository_rule(
    implementation = _realsense_configure_impl,
    environ = [
        _REALSENSE_PATH,
    ],
)
"""Detects and configure the realsense header and library

Add the following to your WORKSPACE FILE:

```python
realsense_configure(name = "local_config_realsense")
```
Args:
    name: A unique name for the workspace rule.
"""
