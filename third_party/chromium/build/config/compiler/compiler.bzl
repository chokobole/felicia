# This is taken from build/compiler/BUILD.gn and modified into bazel form.

load("@cc//:compiler.bzl", "is_clang")

# This will generate warnings when using Clang if code generates exit-time
# destructors, which will slow down closing the program.
# TODO(thakis): Make this a blacklist instead, http://crbug.com/101600
def wexit_time_destructors():
    if is_clang():
        return ["-Wexit-time-destructors"]
    return []
