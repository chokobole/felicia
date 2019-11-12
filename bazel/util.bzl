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
