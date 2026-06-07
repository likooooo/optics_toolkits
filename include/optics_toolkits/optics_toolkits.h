#pragma once

#include "optics_types.h"

#ifdef _WIN32
#  ifdef optics_toolkits_EXPORTS
#    define OPTICS_API __declspec(dllexport)
#  else
#    define OPTICS_API __declspec(dllimport)
#  endif
#else
#  define OPTICS_API __attribute__((visibility("default")))
#endif

#define OPTICS_API_VERSION 1

/** Minimum compatible simulation (core submodule) commit — see README. */
#define OPTICS_TOOLKITS_CORE_MIN_REV "63e0f06fe9047cfafe9a687c4677605f648d9ecb"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Library version string (e.g. "0.1.0"). */
OPTICS_API const char* optics_get_version(void);

/** @brief C ABI version (OPTICS_API_VERSION). */
OPTICS_API int optics_get_api_version(void);

/** @brief Compile-time scalar precision of the shared library. */
OPTICS_API optics_precision_kind optics_get_precision(void);

/** @brief Non-zero if built with OPTICS_ENABLE_PYTHON_CORE_PLUGINS. */
OPTICS_API int optics_has_python_core_plugins(void);

/** @brief Human-readable message for the last error on this thread. */
OPTICS_API const char* optics_last_error(void);

/** @brief Clear the thread-local last-error buffer. */
OPTICS_API void optics_clear_error(void);

#ifdef __cplusplus
}
#endif
