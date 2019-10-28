# Please refer to allocator.gni for detail.

load("@cc//:compiler.bzl", "is_android", "is_arm64", "is_debug", "is_ios", "is_linux", "is_mac", "is_win")
load(
    "//third_party/chromium/build/config/sanitizers:sanitizers.bzl",
    "IS_HWASAN",
)

def _default_allocator():
    if is_android() or is_mac() or is_ios() or is_win() or (is_linux() and is_arm64()):
        return "none"
    else:
        # Disable for a while until figure out protobuf initialization problem.
        # return "tcmalloc"
        return "none"

def _default_use_allocator_shim():
    if is_linux() or is_android() or is_mac() or (is_win() and not is_debug()) and not IS_HWASAN:  # and more
        return True
    else:
        return False

USE_ALLOCATOR = _default_allocator()

USE_ALLOCATOR_SHIM = _default_use_allocator_shim()

USE_PARTITION_ALLOC = not is_ios()

USE_NEW_TCMALLOC = False
