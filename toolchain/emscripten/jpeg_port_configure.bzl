load("//bazel:text_style.bzl", "red")

_EM_CONFIG = "EM_CONFIG"

def _fail(msg):
    """Output failure message when auto configuration fails."""
    fail("%s %s\n" % (red("JPEG Port Confiugation Error:"), msg))

def _jpeg_port_configure_impl(repository_ctx):
    emscripten_cache = repository_ctx.path(repository_ctx.os.environ[_EM_CONFIG]).dirname.get_child(".emscripten_cache")
    jpeg = emscripten_cache.get_child("wasm-obj").get_child("ports-builds").get_child("libjpeg")
    files = repository_ctx.path(jpeg).readdir()
    for f in files:
        if f.basename.endswith(".h"):
            repository_ctx.symlink(f, "jpeg_include/" + f.basename)
        if f.basename.endswith(".a"):
            repository_ctx.symlink(f, "jpeg_lib/" + f.basename)

    repository_ctx.symlink(Label("//toolchain/emscripten:jpeg_port.BUILD"), "BUILD")

jpeg_port_configure = repository_rule(
    implementation = _jpeg_port_configure_impl,
)
"""Detects and configure the jpeg port

Add the following to your WORKSPACE FILE:

```python
jpeg_port_configure(name = "local_config_jpeg_port")
```
Args:
    name: A unique name for the workspace rule.
"""
