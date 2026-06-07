#pragma once

#include <optics_toolkits/optics_tmm.h>
#include "../common/output.h"

#include <math.h>

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

static inline optics_layer OL(double re, double im, double depth)
{
    return (optics_layer){{re, im}, depth};
}

static inline optics_status tmm_last_matrix_s(
    const optics_layer_stack* stack,
    const optics_complex* dir,
    size_t n_dir,
    double wavelength,
    optics_matrix2x2* out)
{
    optics_matrix2x2 buf[16];
    size_t n = sizeof(buf) / sizeof(buf[0]);
    optics_status st = optics_tmm_interface_transfer_matrix_with_thickness_s(
        stack, dir, n_dir, wavelength, buf, &n);
    if (st != OPTICS_OK)
        return st;
    *out = buf[n - 1];
    return OPTICS_OK;
}

static inline optics_status tmm_last_matrix_p(
    const optics_layer_stack* stack,
    const optics_complex* dir,
    size_t n_dir,
    double wavelength,
    optics_matrix2x2* out)
{
    optics_matrix2x2 buf[16];
    size_t n = sizeof(buf) / sizeof(buf[0]);
    optics_status st = optics_tmm_interface_transfer_matrix_with_thickness_p(
        stack, dir, n_dir, wavelength, buf, &n);
    if (st != OPTICS_OK)
        return st;
    *out = buf[n - 1];
    return OPTICS_OK;
}

/** Quadratic interpolation of complex nk vs wavelength (no scipy). */
static inline optics_complex interp_nk_quadratic(const double* wl, const optics_complex* nk, size_t n, double x)
{
    if (n < 3 || x <= wl[0])
        return nk[0];
    if (x >= wl[n - 1])
        return nk[n - 1];
    for (size_t i = 1; i < n - 1; ++i) {
        if (x >= wl[i - 1] && x <= wl[i + 1]) {
            const double x0 = wl[i - 1], x1 = wl[i], x2 = wl[i + 1];
            const double t = (x - x0) / (x2 - x0);
            const double l0 = (x - x1) * (x - x2) / ((x0 - x1) * (x0 - x2));
            const double l1 = (x - x0) * (x - x2) / ((x1 - x0) * (x1 - x2));
            const double l2 = (x - x0) * (x - x1) / ((x2 - x0) * (x2 - x1));
            return (optics_complex){
                l0 * nk[i - 1].re + l1 * nk[i].re + l2 * nk[i + 1].re,
                l0 * nk[i - 1].im + l1 * nk[i].im + l2 * nk[i + 1].im};
        }
    }
    return nk[n / 2];
}
