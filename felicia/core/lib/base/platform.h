#ifndef FELICIA_CORE_PLATFORM_LIB_BASE_H_
#define FELICIA_CORE_PLATFORM_LIB_BASE_H_

// Choose which platform we are on.
#if defined(ANDROID) || defined(__ANDROID__)
#define PLATFORM_ANROID

#elif defined(__APPLE__)
#define PLATFORM_MACOSX
#include "TargetConditionals.h"
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
#define PLATFORM_IOS
#endif

#elif defined(__linux__)
#define PLATFORM_LINUX

#elif defined(_WIN32)
#define PLATFORM_WINDOWS

#else
#define PLATFORM_POSIX

#endif

#if defined(PLATFORM_ANROID) || defined(PLATFORM_MACOSX) || \
    defined(PLATFORM_LINUX)
#define PLATFORM_POSIX
#endif

#if defined(PLATFORM_ANROID) || defined(PLATFORM_IOS)
#define IS_MOBILE_PLATFORM
#endif

// Compiler detection.
#if defined(__GNUC__)
#define COMPILER_GCC
#elif defined(_MSC_VER)
#define COMPILER_MSVC
#else
#error Please add support for your compiler in platform.h
#endif

#if __cplusplus > 201402L
#define LANG_CXX17
#endif
#if __cplusplus > 201103L
#define LANG_CXX14
#endif

// Processor architecture detection.  For more info on what's defined, see:
//   http://msdn.microsoft.com/en-us/library/b0084kay.aspx
//   http://www.agner.org/optimize/calling_conventions.pdf
//   or with gcc, run: "echo | gcc -E -dM -"
#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86_64 1
#define ARCH_CPU_64_BITS 1
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_CPU_X86_FAMILY 1
#define ARCH_CPU_X86 1
#define ARCH_CPU_32_BITS 1
#elif defined(__ARMEL__)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARMEL 1
#define ARCH_CPU_32_BITS 1
#elif defined(__aarch64__) || defined(_M_ARM64)
#define ARCH_CPU_ARM_FAMILY 1
#define ARCH_CPU_ARM64 1
#define ARCH_CPU_64_BITS 1
#else
#error Please add support for your architecture in platform.h
#endif

#endif  // FELICIA_CORE_PLATFORM_LIB_BASE_H_
