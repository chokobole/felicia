CC = %{CC}
MAJOR_VERSION = %{MAJOR_VERSION}
MINOR_VERSION = %{MINOR_VERSION}
OS = %{OS}
ARCH = %{ARCH}

def is_clang():
    return CC == "Clang"

def is_gnuc():
    return CC == "GNUC"

def is_msvc():
    return CC == "MSVC"

def is_android():
    return OS == "Android"

def is_mac():
    return OS == "MACOSX"

def is_ios():
    return OS == "IOS"

def is_linux():
    return OS == "Linux"

def is_win():
    return OS == "Windows"

def is_x64():
    return ARCH == "x86_64"

def is_x86():
    return ARCH == "x86"

def is_arm():
    return ARCH == "armel"

def is_arm64():
    return ARCH == "arm64"

def is_component_build():
    return False

# TODO: how to figure out if it is a debug?
def is_debug():
    return True