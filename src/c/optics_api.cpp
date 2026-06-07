#include <optics_toolkits/optics_toolkits.h>

#include "internal/exception_bridge.hpp"

#include <string>

using optics::internal::g_last_error;

extern "C" {

const char* optics_get_version(void)
{
    return "0.1.0";
}

int optics_get_api_version(void)
{
    return OPTICS_API_VERSION;
}

optics_precision_kind optics_get_precision(void)
{
#if defined(OPTICS_TOOLKITS_F32)
    return OPTICS_PRECISION_F32;
#else
    return OPTICS_PRECISION_F64;
#endif
}

int optics_has_python_core_plugins(void)
{
#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
    return 1;
#else
    return 0;
#endif
}

const char* optics_last_error(void)
{
    return g_last_error.c_str();
}

void optics_clear_error(void)
{
    g_last_error.clear();
}

} // extern "C"
