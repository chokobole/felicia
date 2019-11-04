#include <stdio.h>

#include "../../third_party/chromium/build/build_config.h"

int main() {
#if defined(__clang__)
  const char compiler_id[] = "Clang";
  const int compiler_version_major = __clang_major__;
  const int compiler_version_minor = __clang_minor__;
#elif defined(__GNUC__)
  const char compiler_id[] = "GNUC";
  const int compiler_version_major = __GNUC__;
  const int compiler_version_minor = __GNUC_MINOR__;
#elif defined(_MSC_VER)
  const char compiler_id[] = "MSVC";
  const int compiler_version_major = _MSC_VER / 100;
  const int compiler_version_minor = _MSC_VER % 100;
#else
#error Please add support for your compiler
#endif

#if defined(OS_ANDROID)
  const char os_name[] = "Android";
#elif defined(OS_IOS)
  const char os_name[] = "IOS";
#elif defined(OS_MACOSX)
  const char os_name[] = "MACOSX";
#elif defined(OS_LINUX)
  const char os_name[] = "Linux";
#elif defined(OS_WIN)
  const char os_name[] = "Windows";
#else
#error Please add support for your os
#endif

#if defined(ARCH_CPU_X86_64)
  const char arch[] = "x86_64";
#elif defined(ARCH_CPU_X86)
  const char arch[] = "x86";
#elif defined(ARCH_CPU_ARMEL)
  const char arch[] = "armel";
#elif defined(ARCH_CPU_ARM64)
  const char arch[] = "arm64";
#error Please add support for your architecture
#endif

  printf("%s %i %i %s %s\n", compiler_id, compiler_version_major,
         compiler_version_minor, os_name, arch);

  return 0;
}