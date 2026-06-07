#pragma once

#include <optics_toolkits/optics_toolkits.h>

#include <exception>
#include <stdexcept>
#include <string>

namespace optics::internal {

inline thread_local std::string g_last_error;

inline void set_error(std::string msg)
{
    g_last_error = std::move(msg);
}

inline optics_status guard(auto&& fn)
{
    try {
        g_last_error.clear();
        fn();
        return OPTICS_OK;
    } catch (const std::invalid_argument& e) {
        set_error(e.what());
        return OPTICS_ERR_INVALID_ARG;
    } catch (const std::bad_alloc&) {
        set_error("out of memory");
        return OPTICS_ERR_OUT_OF_MEMORY;
    } catch (const std::exception& e) {
        set_error(e.what());
        return OPTICS_ERR_INTERNAL;
    } catch (...) {
        set_error("unknown error");
        return OPTICS_ERR_INTERNAL;
    }
}

} // namespace optics::internal
