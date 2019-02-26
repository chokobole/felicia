def if_windows(a):
    return select({
        "//felicia:windows": a,
        "//conditions:default": [],
    })

def if_not_windows(a):
    return select({
        "//felicia:windows": [],
        "//conditions:default": a,
    })

def if_linux(a):
    return select({
        "//felicia:linux_x86_64": a,
        "//felicia:linux_ppc64le": a,
        "//felicia:linux_s390x": a,
        "//conditions:default": [],
    })

def if_darwin(a):
    return select({
        "//felicia:darwin": a,
        "//felicia:darwin_x86_64": a,
        "//conditions:default": [],
    })

def if_not_darwin(a):
    return select({
        "//felicia:darwin": [],
        "//felicia:darwin_x86_64": [],
        "//conditions:default": a,
    })

def if_freebsd(a):
    return select({
        "//felicia:freebsd": a,
        "//conditions:default": [],
    })

def fel_win_copts(is_external = False):
    WINDOWS_COPTS = [
        "/std:c++14",
        "/Ithird_party/chromium",
    ]
    if is_external:
        return WINDOWS_COPTS + ["/UFEL_COMPILE_LIBRARY"]
    else:
        return WINDOWS_COPTS + ["/DFEL_COMPILE_LIBRARY"]


def fel_copts(is_external = False):
    return if_not_windows([
        "-Ithird_party/chromium",
    ]) + select({
        "//felicia:windows": fel_win_copts(is_external),
        "//conditions:default": [
            "-std=c++14",
        ],
    })