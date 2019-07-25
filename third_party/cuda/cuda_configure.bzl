load(
    "//bazel:felicia_repository.bzl",
    "get_bin_path",
    "is_executable",
    "symlink_genrule_for_dir",
)
load("//bazel:felicia_util.bzl", "red")

_CUDA_PATH = "CUDA_PATH"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("Cuda Confiugation Error:"), msg))

def _cuda_configure_impl(repository_ctx):
    if not is_executable(repository_ctx, "nvcc"):
        _fail("nvcc doesn't exist or is not executable.")
    nvcc_bin_path = get_bin_path(repository_ctx, "nvcc")
    cmd = [nvcc_bin_path, "-v", "/dev/null", "-o", "/dev/null"]
    result = repository_ctx.execute(cmd)

    target_size = None
    top = None
    TARGET_SIZE_PREFIX = "#$ _TARGET_SIZE_="
    TOP_PREFIX = "#$ TOP="

    # On windows prints to stdout, on linux prints to stderr, though.
    result_txt = result.stderr if len(result.stderr) > 0 else result.stdout
    for line in result_txt.splitlines():
        if line.startswith(TARGET_SIZE_PREFIX):
            target_size_str = line[len(TARGET_SIZE_PREFIX):]
            if len(target_size_str) == 0:
                continue
            target_size = int(target_size_str)
        if line.startswith(TOP_PREFIX):
            top = line[len(TOP_PREFIX):]

    if top == None:
        _fail("Failed to configure top")

    if target_size == None:
        _fail("Failed to configure target_size")

    cuda_include_rule = symlink_genrule_for_dir(
        repository_ctx,
        top + "/include",
        "cuda_include",
        "cuda_include",
    )

    repository_ctx.template(
        "BUILD",
        Label("//third_party/cuda:BUILD.tpl"),
        {
            "%{CUDA_INCLUDE_GENRULE}": cuda_include_rule,
        },
    )

cuda_configure = repository_rule(
    implementation = _cuda_configure_impl,
    environ = [
        _CUDA_PATH,
    ],
)
"""Detects and configure the cuda header and library

Add the following to your WORKSPACE FILE:

```python
cuda_configure(name = "local_config_cuda")
```
Args:
    name: A unique name for the workspace rule.
"""
