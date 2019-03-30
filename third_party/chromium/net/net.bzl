load("@cc//:compiler.bzl", "is_win")

def net_copts(is_external = False):
    return []

def net_linkopts():
    if is_win():
        return [
            "Iphlpapi.lib",
            "Ws2_32.lib",
        ]
    return []
