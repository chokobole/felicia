def red(msg):
    red = "\033[0;31m"
    no_color = "\033[0m"
    return "%s%s%s" % (red, msg, no_color)

def norm_path(path):
    """Returns a path with '/' and remove the trailing slash."""
    path = path.replace("\\", "/")
    if path[-1] == "/":
        path = path[:-1]
    return path
