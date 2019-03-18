load(
    "@bazel_tools//tools/cpp:lib_cc_configure.bzl",
    "auto_configure_fail",
    "get_cpu_value",
    "escape_string",
)
load(
    "@bazel_tools//tools/cpp:windows_cc_configure.bzl",
    "find_llvm_path",
    "find_llvm_tool",
    "find_msvc_tool",
    "find_vc_path",
    "setup_vc_env_vars",
)

def _use_clang_cl(repository_ctx):
    """Returns True if USE_CLANG_CL is set to 1."""
    return repository_ctx.os.environ.get("USE_CLANG_CL", default = "0") == "1"

def _configure_windows_tool_path(repository_ctx):
    if _use_clang_cl(repository_ctx):
        llvm_path = find_llvm_path(repository_ctx)
        if not llvm_path:
            auto_configure_fail("\nUSE_CLANG_CL is set to 1, but Bazel cannot find Clang installation on your system.\n" +
                                "Please install Clang via http://releases.llvm.org/download.html\n")
        return find_llvm_tool(repository_ctx, llvm_path, "clang-cl.exe"), None
    else:
        vc_path = find_vc_path(repository_ctx)
        env = setup_vc_env_vars(repository_ctx, vc_path)
        escaped_include_paths = escape_string(env["INCLUDE"])
        escaped_lib_paths = escape_string(env["LIB"])
        flags = []
        for path in escaped_include_paths.split(";"):
            if path:
                flags.append("/I%s" % path)
        flags.extend(["/link LIBCMT.lib"])
        for path in escaped_lib_paths.split(";"):
            if path:
                flags.append("/LIBPATH:%s" % path)


        return find_msvc_tool(repository_ctx, vc_path, "cl.exe"), flags

def _cc_autoconf_impl(repository_ctx):
    """Implementation of the cc_autoconf repository rule."""
    repository_ctx.symlink(Label("//third_party/cc:BUILD"), "BUILD")

    env = repository_ctx.os.environ
    cpu_value = get_cpu_value(repository_ctx)
    flags = None

    if "BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN" in env and env["BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN"] == "1":
        fail("Failed to detect cpp toolchain.")
    elif cpu_value == "freebsd":
        tool_path = "/usr/bin/clang"
    elif cpu_value == "x64_windows":
        tool_path, flags = _configure_windows_tool_path(repository_ctx)
    else:
        tool_path = repository_ctx.path(Label("@local_config_cc//:cc_wrapper.sh"))

    executable = repository_ctx.path("build_config")
    source = repository_ctx.path(Label("//third_party/cc:build_config.cc"))
    if flags != None:
        # MSVC
        cmd = [tool_path, source]
        cmd.extend(flags)
        cmd.append("/Fe%s" % executable)
    else:
        cmd = [tool_path, source, "-o", executable]
    result = repository_ctx.execute(cmd)
    if result.return_code != 0:
        fail("Failed to compile build_config.", result.stdout)
    result = repository_ctx.execute([executable])
    if result.return_code != 0:
        fail("Failed to run build_config.", result.stderr)
    output = result.stdout.strip().split(" ")

    repository_ctx.template(
        "compiler.bzl",
        Label("//third_party/cc:compiler.bzl.tpl"),
        {
            "%{CC}": "\"%s\"" % output[0],
            "%{MAJOR_VERSION}": output[1],
            "%{MINOR_VERSION}": output[2],
        }
    )

cc_configure = repository_rule(
    implementation = _cc_autoconf_impl,
    environ = [
        "ABI_LIBC_VERSION",
        "ABI_VERSION",
        "BAZEL_COMPILER",
        "BAZEL_HOST_SYSTEM",
        "BAZEL_LINKOPTS",
        "BAZEL_PYTHON",
        "BAZEL_SH",
        "BAZEL_TARGET_CPU",
        "BAZEL_TARGET_LIBC",
        "BAZEL_TARGET_SYSTEM",
        "BAZEL_USE_CPP_ONLY_TOOLCHAIN",
        "BAZEL_DO_NOT_DETECT_CPP_TOOLCHAIN",
        "BAZEL_USE_LLVM_NATIVE_COVERAGE",
        "BAZEL_VC",
        "BAZEL_VS",
        "BAZEL_LLVM",
        "USE_CLANG_CL",
        "CC",
        "CC_CONFIGURE_DEBUG",
        "CC_TOOLCHAIN_NAME",
        "CPLUS_INCLUDE_PATH",
        "GCOV",
        "HOMEBREW_RUBY_PATH",
        "SYSTEMROOT",
        "VS90COMNTOOLS",
        "VS100COMNTOOLS",
        "VS110COMNTOOLS",
        "VS120COMNTOOLS",
        "VS140COMNTOOLS",
    ]
)
"""Detects and configure the compiler for C++

Add the following to your WORKSPACE FILE:

```python
cc_configure(name = "local_config_cc")
```

Args:
    name: A unique name for the workspace rule.
"""