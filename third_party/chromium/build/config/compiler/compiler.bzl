# Please refer to compiler.gni for detail.

load("@cc//:compiler.bzl", "is_android", "is_arm", "is_arm64", "is_clang", "is_debug", "is_ios", "is_linux", "is_mac", "is_win", "is_x64", "is_x86")
load("//third_party/chromium/build/config/sanitizers:sanitizers.bzl", "USING_SANITIZER")
load("//third_party/chromium/build/config:buildconfig.bzl", "IS_OFFICIAL_BUILD")

############################################### compiler.gni ###############################################
ENABLE_PROFILING = False

EXCLUDE_UNWIND_TABLES = IS_OFFICIAL_BUILD or not is_debug()  # and more

OPTIMIZER_FOR_SIZE = is_android() or is_ios()  # and more

def _enable_frame_pointers():
    if is_mac() or is_ios() or is_linux():
        return True
    elif is_win():
        return not is_x64()
    elif is_android():
        return ENABLE_PROFILING or is_arm64() or (is_x86() and not EXCLUDE_UNWIND_TABLES and OPTIMIZER_FOR_SIZE) or USING_SANITIZER
    else:
        return USING_SANITIZER or ENABLE_PROFILING or is_debug()

ENABLE_FRAME_POINTERS = _enable_frame_pointers()

def _can_unwind_with_frame_pointers():
    if is_arm() and True:  # arm_use_thumb()
        return False
    elif is_win():
        return False
    else:
        return _enable_frame_pointers()

CAN_UNWIND_WITH_FRAME_POINTERS = _can_unwind_with_frame_pointers()
CAN_UNWIND_WITH_CFI_TABLE = is_android() and not ENABLE_FRAME_POINTERS and is_arm()

USE_LLD = is_clang() and (is_win() or is_android() or (is_linux() and not is_x86()))
