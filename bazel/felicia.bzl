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

def if_not_linux(a):
    return select({
        "//felicia:linux_x86_64": [],
        "//felicia:linux_ppc64le": [],
        "//felicia:linux_s390x": [],
        "//conditions:default": a,
    })

# This is temporary to support build third_party/chromium/net/base
def if_not_windows_and_not_linux(a):
    return select({
        "//felicia:linux_x86_64": [],
        "//felicia:linux_ppc64le": [],
        "//felicia:linux_s390x": [],
        "//felicia:windows": [],
        "//conditions:default": a,
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

def if_static(extra_deps, otherwise = []):
    return select({
        "//felicia:framework_shared_object": otherwise,
        "//conditions:default": extra_deps,
    })
