#pragma once

#include "optics_toolkits.h"

#ifdef __cplusplus
extern "C" {
#endif

OPTICS_API optics_status optics_py_init(const char* const* plugin_dirs, size_t n);
OPTICS_API void optics_py_shutdown(void);

OPTICS_API optics_status optics_viz_plot_xy(
    const double* x,
    const double* y,
    size_t n,
    const char* title,
    const char* xlabel,
    const char* ylabel,
    const char* style);

OPTICS_API optics_status optics_viz_plot_multi_xy(
    const double* const* y_sets,
    size_t n_curves,
    size_t n_points,
    double x0,
    double x_step,
    const char* const* labels,
    const char* const* styles,
    const char* title,
    const char* xlabel,
    const char* ylabel);

OPTICS_API optics_status optics_viz_imshow(
    const double* data,
    const size_t* shape,
    size_t ndim,
    const char* title);

#ifdef __cplusplus
}
#endif
