def red(msg):
    red = "\033[0;31m"
    no_color = "\033[0m"
    return "%s%s%s" % (red, msg, no_color)

def norm_path(path):
    """Returns a path with '/' and remove the trailing slash."""
    path = path.replace("\\", "/")
    if path[-1] == "/":
        path = path[:-1]
    path_elms = []
    for path_elm in path.split("/"):
        if path_elm == "..":
            if len(path_elms) > 0:
                path_elms.pop()
            continue
        path_elms.append(path_elm)
    return "/".join(path_elms)

def append_to_set(s, element):
    has_element = False
    for e in s:
        if e == element:
            has_element = True
            break
    if has_element == False:
        s.append(element)

def extend_to_set(s, element):
    for e in element:
        append_to_set(s, e)
