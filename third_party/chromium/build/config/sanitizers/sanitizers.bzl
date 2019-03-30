# Please refer to sanitizers.gni for detail.

load("@cc//:compiler.bzl", "is_linux", "is_x64")
load("//third_party/chromium/build/config:buildconfig.bzl", "IS_OFFICIAL_BUILD")

IS_HWASAN = False

IS_CFI = is_linux() and is_x64()
USE_CFI_CAST = False
USE_CFI_ICALL = is_linux() and is_x64() and IS_OFFICIAL_BUILD
USE_CFI_DIAG = False
USE_CFI_RECOVER = False

USING_SANITIZER = IS_HWASAN or USE_CFI_DIAG  # and more
