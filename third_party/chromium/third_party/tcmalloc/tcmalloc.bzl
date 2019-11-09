load("@cc//:compiler.bzl", "is_android", "is_clang", "is_debug", "is_linux")
load(
    "//third_party/chromium/build/config:allocator.bzl",
    "USE_ALLOCATOR",
    "USE_ALLOCATOR_SHIM",
    "USE_NEW_TCMALLOC",
)
load(
    "//third_party/chromium/build/config/compiler:compiler.bzl",
    "ENABLE_PROFILING",
)

ENABLE_DEBUGALLOCATION = is_debug()
USE_TCMALLOC_SMALL_BUT_SLOW = False

def tcmalloc_dir():
    if USE_NEW_TCMALLOC:
        return "tcmalloc/chromium"
    else:
        return "gperftools-2.0/chromium"

def tcmalloc_copts():
    return []

def tcmalloc_defines():
    defines = ["TCMALLOC_USE_DOUBLYLINKED_FREELIST", "TCMALLOC_DISABLE_HUGE_ALLOCATIONS"]

    if ENABLE_DEBUGALLOCATION:
        defines.append("TCMALLOC_FOR_DEBUGALLOCATION")

    if USE_ALLOCATOR_SHIM:
        defines.append("TCMALLOC_DONT_REPLACE_SYSTEM_ALLOC")

    if USE_TCMALLOC_SMALL_BUT_SLOW:
        defines.append("TCMALLOC_SMALL_BUT_SLOW")

    defines.append("NO_HEAP_CHECK")

    if ENABLE_PROFILING:
        defines.append("ENABLE_PROFILING=1")

    return defines

def tcmalloc_linkopts():
    linkopts = [
        "-Wl,-uIsHeapProfilerRunning,-uProfilerStart",
        "-Wl,-u_Z21InitialMallocHook_NewPKvj,-u_Z22InitialMallocHook_MMapPKvS0_jiiix,-u_Z22InitialMallocHook_SbrkPKvi",
        "-Wl,-u_Z21InitialMallocHook_NewPKvm,-u_Z22InitialMallocHook_MMapPKvS0_miiil,-u_Z22InitialMallocHook_SbrkPKvl",
        "-Wl,-u_ZN15HeapLeakChecker12IgnoreObjectEPKv,-u_ZN15HeapLeakChecker14UnIgnoreObjectEPKv",
    ]
    return select({
        "@com_github_chokobole_felicia//felicia:android": linkopts,
        "@com_github_chokobole_felicia//felicia:linux": linkopts,
        "//conditions:default": [],
    })

def tcmalloc_additional_srcs():
    srcs = []
    if USE_NEW_TCMALLOC:
        srcs += [
            "%s/src/emergency_malloc_for_stacktrace.cc" % tcmalloc_dir(),
            "%s/src/fake_stacktrace_scope.cc" % tcmalloc_dir(),
        ]
    if ENABLE_PROFILING:
        srcs += [
            "%s/src/base/thread_lister.cc" % tcmalloc_dir(),
            "%s/src/profile-handler.cc" % tcmalloc_dir(),
            "%s/src/profiledata.cc" % tcmalloc_dir(),
            "%s/src/profiler.cc" % tcmalloc_dir(),
        ]
    return srcs

def tcmalloc_additional_hdrs():
    hdrs = []
    if not USE_NEW_TCMALLOC:
        hdrs.append("%s/src/base/cycleclock.h" % tcmalloc_dir())
    if ENABLE_PROFILING:
        hdrs += [
            "%s/src/base/thread_lister.h" % tcmalloc_dir(),
            "%s/src/profile-handler.h" % tcmalloc_dir(),
            "%s/src/profiledata.h" % tcmalloc_dir(),
        ]
    return hdrs
