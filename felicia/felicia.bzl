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

def if_linux_x86_64(a):
    return select({
        "//felicia:linux_x86_64": a,
        "//conditions:default": [],
    })

def if_darwin(a):
    return select({
        "//felicia:darwin": a,
        "//felicia:darwin_x86_64": a,
        "//conditions:default": [],
    })

def fel_copts():
    return  if_darwin(["-fno-objc-arc"]) + []