def _opencv_configure_impl(repository_ctx):
    repository_ctx.template(
        "BUILD",
        Label("//third_party/opencv:BUILD.tpl"),
        {},
    )

opencv_configure = repository_rule(
    implementation = _opencv_configure_impl,
)
"""Detects and configure the opencv header and library

Add the following to your WORKSPACE FILE:

```python
opencv_configure(name = "local_config_opencv")
```
Args:
    name: A unique name for the workspace rule.
"""
