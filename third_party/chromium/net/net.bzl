load("@cc//:compiler.bzl", "is_win")

def net_copts():
    return []

def net_defines():
    return []

def net_includes():
    return []

def net_linkopts():
    if is_win():
        return [
            "Iphlpapi.lib",
            "Ws2_32.lib",
        ]
    return []
