#ifndef FELICIA_CORE_LIB_BASE_EXPORT_H_
#define FELICIA_CORE_LIB_BASE_EXPORT_H_

#if defined(_WIN32)
#ifdef FEL_COMPILE_LIBRARY
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif  // FEL_COMPILE_LIBRARY

#else

#ifdef FEL_COMPILE_LIBRARY
#define EXPORT __attribute__((visibility("default")))
#else
#define EXPORT
#endif  // FEL_COMPILE_LIBRARY

#endif  // _WIN32

#endif  // FELICIA_CORE_LIB_BASE_EXPORT_H_