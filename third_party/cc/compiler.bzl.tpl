CC = %{CC}
MAJOR_VERSION = %{MAJOR_VERSION}
MINOR_VERSION = %{MINOR_VERSION}

def is_clang():
  return CC == "Clang"