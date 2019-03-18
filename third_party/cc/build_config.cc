#include <stdio.h>

int main() {
#if defined(__clang__)
  const char compiler_id[] = "Clang";
  const int compiler_version_major = __clang_major__;
  const int compiler_version_minor = __clang_minor__;
#elif defined(__GNUC__)
  const char compiler_id[] = "GNU";
  const int compiler_version_major = __GNUC__;
  const int compiler_version_minor = __GNUC_MINOR__;
#elif defined(_MSC_VER)
  const char compiler_id[] = "MSVC";
  const int compiler_version_major = _MSC_VER / 100;
  const int compiler_version_minor = _MSC_VER % 100;
#else
#error Please add support for your compiler
#endif

  printf("%s %i %i", compiler_id, compiler_version_major,
         compiler_version_minor);
  return 0;
}