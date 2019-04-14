NODE_VERSION=%{NODE_VERSION}

def define(flags):
    window_defines = ["/D" + flag for flag in flags]
    default_defines = ["-D" + flag for flag in flags]
    return select({
        "//:windows": window_defines,
        "//conditions:default": default_defines,
    })

def include(flags):
    window_includes = ["/I" + flag for flag in flags]
    default_includes = ["-I" + flag for flag in flags]
    return select({
        "//:windows": window_includes,
        "//conditions:default": default_includes,
    })

def node_addon_api_copts():
    return define([
        "EXTERNAL_API",
        "USING_UV_SHARED=1",
        "USING_V8_SHARED=1",
        "V8_DEPRECATION_WARNINGS=1",
        "_LARGEFILE_SOURCE",
        "_FILE_OFFSET_BITS=64",
        "NAPI_DISABLE_CPP_EXCEPTIONS",
        "BUILDING_NODE_EXTENSION"
    ]) + [
        "-fno-rtti", "-fno-exceptions"
    ]