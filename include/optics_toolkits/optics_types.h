#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Complex number with double fields (API-stable across f32/f64 builds). */
typedef struct optics_complex {
    double re;
    double im;
} optics_complex;

/** @brief Single layer: refractive index and physical thickness. */
typedef struct optics_layer {
    optics_complex nk;
    double depth;
} optics_layer;

/** @brief 2x2 complex transfer matrix (row-major). */
typedef struct optics_matrix2x2 {
    optics_complex m[2][2];
} optics_matrix2x2;

/** @brief 3-component complex vector (e.g. E-field). */
typedef struct optics_vec3 {
    optics_complex x;
    optics_complex y;
    optics_complex z;
} optics_vec3;

typedef enum optics_status {
    OPTICS_OK = 0,
    OPTICS_ERR_INVALID_ARG,
    OPTICS_ERR_OUT_OF_MEMORY,
    OPTICS_ERR_INTERNAL,
    OPTICS_ERR_NOT_AVAILABLE
} optics_status;

typedef enum optics_precision_kind {
    OPTICS_PRECISION_F32 = 0,
    OPTICS_PRECISION_F64 = 1
} optics_precision_kind;

#ifdef __cplusplus
}
#endif
