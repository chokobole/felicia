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
        "USING_UV_SHARED=1",
        "USING_V8_SHARED=1",
        "V8_DEPRECATION_WARNINGS=1",
        "NAPI_DISABLE_CPP_EXCEPTIONS",
        "BUILDING_NODE_EXTENSION"
    ]) + select({
        "//:windows": [
            "/GS",
            "/DWIN32",
            "/D_CRT_SECURE_NO_DEPRECATE",
            "/D_CRT_NONSTDC_NO_DEPRECATE",
            "/GR-",
            "/GF",
            "/D_HAS_EXCEPTIONS=0"
        ],
        "//conditions:default": [
            "-D_LARGEFILE_SOURCE",
            "-D_FILE_OFFSET_BITS=64",
            "-fvisibility=hidden",
            "-fno-rtti",
            "-fno-exceptions"
        ],
    })