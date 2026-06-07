#include <optics_toolkits/optics_viz.h>

#include "internal/exception_bridge.hpp"

#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
#  include <py_helper.hpp>
#  include <py_plugin.h>
#endif

#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
std::once_flag g_py_once;

void ensure_py(const char* const* plugin_dirs, size_t n)
{
    std::call_once(g_py_once, [&] {
        py_engine::init(false);
        if (n > 0 && plugin_dirs) {
            std::vector<std::string> paths;
            for (size_t i = 0; i < n; ++i) {
                if (plugin_dirs[i] && plugin_dirs[i][0])
                    paths.emplace_back(plugin_dirs[i]);
            }
            if (!paths.empty())
                py_plugin::paths = std::move(paths);
        }
    });
}
#endif

} // namespace

extern "C" {

optics_status optics_py_init(const char* const* plugin_dirs, size_t n)
{
#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
    return optics::internal::guard([&] { ensure_py(plugin_dirs, n); });
#else
    (void)plugin_dirs;
    (void)n;
    return OPTICS_ERR_NOT_AVAILABLE;
#endif
}

void optics_py_shutdown(void)
{
#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
    py_engine::dispose();
#endif
}

optics_status optics_viz_plot_xy(
    const double* x,
    const double* y,
    size_t n,
    const char* title,
    const char* xlabel,
    const char* ylabel,
    const char* style)
{
#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
    return optics::internal::guard([&] {
        if (!x || !y || n == 0)
            throw std::invalid_argument("invalid plot data");
        ensure_py(nullptr, 0);
        std::vector<float> xs(n), ys(n);
        for (size_t i = 0; i < n; ++i) {
            xs[i] = static_cast<float>(x[i]);
            ys[i] = static_cast<float>(y[i]);
        }
        plot_curves(std::vector<std::vector<float>>{ys},
            std::vector<std::vector<float>>{xs},
            std::vector<std::string>{ylabel ? ylabel : "y"},
            std::vector<std::string>{style ? style : "b-"},
            1.f, title ? title : "", xlabel ? xlabel : "X", ylabel ? ylabel : "Y");
    });
#else
    (void)x;
    (void)y;
    (void)n;
    (void)title;
    (void)xlabel;
    (void)ylabel;
    (void)style;
    return OPTICS_ERR_NOT_AVAILABLE;
#endif
}

optics_status optics_viz_plot_multi_xy(
    const double* const* y_sets,
    size_t n_curves,
    size_t n_points,
    double x0,
    double x_step,
    const char* const* labels,
    const char* const* styles,
    const char* title,
    const char* xlabel,
    const char* ylabel)
{
#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
    return optics::internal::guard([&] {
        if (!y_sets || n_curves == 0 || n_points == 0)
            throw std::invalid_argument("invalid plot data");
        ensure_py(nullptr, 0);
        std::vector<std::vector<float>> curves(n_curves);
        std::vector<std::string> legends(n_curves);
        std::vector<std::string> plot_styles(n_curves);
        for (size_t c = 0; c < n_curves; ++c) {
            curves[c].resize(n_points);
            for (size_t i = 0; i < n_points; ++i)
                curves[c][i] = static_cast<float>(y_sets[c][i]);
            legends[c] = labels && labels[c] ? labels[c] : "curve";
            plot_styles[c] = styles && styles[c] ? styles[c] : "b-";
        }
        plot_curves(curves,
            std::vector<float>(n_curves, static_cast<float>(x0)),
            std::vector<float>(n_curves, static_cast<float>(x_step)),
            legends, plot_styles, 1.f,
            title ? title : "", xlabel ? xlabel : "X", ylabel ? ylabel : "Y");
    });
#else
    (void)y_sets;
    (void)n_curves;
    (void)n_points;
    (void)x0;
    (void)x_step;
    (void)labels;
    (void)styles;
    (void)title;
    (void)xlabel;
    (void)ylabel;
    return OPTICS_ERR_NOT_AVAILABLE;
#endif
}

optics_status optics_viz_imshow(
    const double* data,
    const size_t* shape,
    size_t ndim,
    const char* title)
{
#if defined(OPTICS_HAS_PYTHON_CORE_PLUGINS)
    return optics::internal::guard([&] {
        if (!data || !shape || ndim == 0)
            throw std::invalid_argument("invalid image data");
        ensure_py(nullptr, 0);
        size_t count = 1;
        std::vector<size_t> dim(ndim);
        for (size_t i = 0; i < ndim; ++i) {
            dim[i] = shape[i];
            count *= shape[i];
        }
        std::vector<double> buf(data, data + count);
        imshow(buf, dim);
        (void)title;
    });
#else
    (void)data;
    (void)shape;
    (void)ndim;
    (void)title;
    return OPTICS_ERR_NOT_AVAILABLE;
#endif
}

} // extern "C"
