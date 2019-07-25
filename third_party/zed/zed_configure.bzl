load(
    "//bazel:felicia_repository.bzl",
    "is_linux",
    "is_windows",
    "symlink_genrule_for_dir",
)
load("//bazel:felicia_util.bzl", "red")

_ZED_PATH = "ZED_PATH"
_DEFAULT_PATH_ON_LINUX = "/usr/local/zed"
_DEFAULT_PATH_ON_WINDOWS = "C:\\Program Files (x86)\\ZED SDK"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("Zed Confiugation Error:"), msg))

def _get_zed_path(repository_ctx):
    zed_path = repository_ctx.os.environ.get(_ZED_PATH)
    if zed_path == None:
        if is_linux(repository_ctx):
            zed_path = _DEFAULT_PATH_ON_LINUX
        elif is_windows(repository_ctx):
            zed_path = _DEFAULT_PATH_ON_WINDOWS

    if repository_ctx.path(zed_path).exists:
        return zed_path
    else:
        _fail("Failed to find the path for zed, Did you installed sdk " +
              "by default? or please locate the sdk path setting " +
              "enviornment variable \"ZED_PATH\"")

    return None

def _zed_configure_impl(repository_ctx):
    zed_include_rule = ""
    zed_import_lib_genrule = ""
    zed_import_dll_genrule = ""

    path = _get_zed_path(repository_ctx)
    zed_include = path + "/include"
    zed_lib = path + "/lib"
    zed_include_rule = symlink_genrule_for_dir(
        repository_ctx,
        zed_include,
        "zed_include",
        "zed_include",
    )
    zed_import_lib_genrule = symlink_genrule_for_dir(
        repository_ctx,
        zed_lib,
        "zed_lib",
        "zed_lib",
    )

    if is_windows(repository_ctx):
        zed_bin = path + "/bin"
        zed_import_dll_genrule = symlink_genrule_for_dir(
            repository_ctx,
            zed_bin,
            "zed_bin",
            "zed_bin",
        )

    repository_ctx.template(
        "BUILD",
        Label("//third_party/zed:BUILD.tpl"),
        {
            "%{ZED_INCLUDE_GENRULE}": zed_include_rule,
            "%{ZED_IMPORT_LIB_GENRULE}": zed_import_lib_genrule,
            "%{ZED_IMPORT_DLL_GENRULE}": zed_import_dll_genrule,
        },
    )

zed_configure = repository_rule(
    implementation = _zed_configure_impl,
    environ = [
        _ZED_PATH,
    ],
)
"""Detects and configure the zed header and library

Add the following to your WORKSPACE FILE:

```python
zed_configure(name = "local_config_zed")
```
Args:
    name: A unique name for the workspace rule.
"""
