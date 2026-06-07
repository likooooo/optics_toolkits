#pragma once

#include <optics_toolkits/optics_toolkits.h>
#include <optics_toolkits/optics_viz.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline int optics_ensure_output_dir(const char* path)
{
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "mkdir -p %s", path);
    return system(cmd);
}

static inline int optics_write_csv_2col(const char* path, const char* x_name, const char* y_name,
    const double* x, const double* y, size_t n)
{
    FILE* f = fopen(path, "w");
    if (!f)
        return -1;
    fprintf(f, "%s,%s\n", x_name, y_name);
    for (size_t i = 0; i < n; ++i)
        fprintf(f, "%.17g,%.17g\n", x[i], y[i]);
    fclose(f);
    return 0;
}

static inline int optics_write_csv_multi(const char* path, const char* x_name,
    const char* const* y_names, size_t n_curves,
    const double* x, const double* const* y, size_t n)
{
    FILE* f = fopen(path, "w");
    if (!f)
        return -1;
    fprintf(f, "%s", x_name);
    for (size_t c = 0; c < n_curves; ++c)
        fprintf(f, ",%s", y_names[c]);
    fprintf(f, "\n");
    for (size_t i = 0; i < n; ++i) {
        fprintf(f, "%.17g", x[i]);
        for (size_t c = 0; c < n_curves; ++c)
            fprintf(f, ",%.17g", y[c][i]);
        fprintf(f, "\n");
    }
    fclose(f);
    return 0;
}

static inline void optics_output_init_viz(const char* plugin_dir)
{
    if (!optics_has_python_core_plugins())
        return;
    const char* dirs[] = {plugin_dir};
    optics_py_init(dirs, plugin_dir ? 1 : 0);
}

static inline void optics_output_plot_xy_or_csv(
    const char* csv_path,
    const double* x, const double* y, size_t n,
    const char* title, const char* xlabel, const char* ylabel, const char* style)
{
    if (optics_has_python_core_plugins()) {
        optics_viz_plot_xy(x, y, n, title, xlabel, ylabel, style);
    } else {
        optics_ensure_output_dir("output");
        optics_write_csv_2col(csv_path, xlabel, ylabel, x, y, n);
        printf("wrote %s\n", csv_path);
    }
}

static inline void optics_output_plot_multi_or_csv(
    const char* csv_path,
    const double* x,
    const double* const* y_sets,
    size_t n_curves,
    size_t n_points,
    const char* const* labels,
    const char* const* styles,
    const char* title,
    const char* xlabel,
    const char* ylabel)
{
    if (optics_has_python_core_plugins()) {
        double x0 = n_points > 0 ? x[0] : 0.0;
        double x_step = n_points > 1 ? (x[1] - x[0]) : 1.0;
        optics_viz_plot_multi_xy(y_sets, n_curves, n_points, x0, x_step, labels, styles, title, xlabel, ylabel);
    } else {
        optics_ensure_output_dir("output");
        optics_write_csv_multi(csv_path, xlabel, labels, n_curves, x, y_sets, n_points);
        printf("wrote %s\n", csv_path);
    }
}
