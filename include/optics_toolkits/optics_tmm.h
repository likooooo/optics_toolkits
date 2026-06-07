#pragma once

#include "optics_toolkits.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct optics_layer_stack optics_layer_stack;

/** @name Layer stack */
/** @{ */
OPTICS_API optics_layer_stack* optics_layer_stack_create(void);
OPTICS_API optics_status optics_layer_stack_add_layer(optics_layer_stack* stack, optics_layer layer);
OPTICS_API size_t optics_layer_stack_layer_count(const optics_layer_stack* stack);
OPTICS_API optics_status optics_layer_stack_get_layer(const optics_layer_stack* stack, size_t index, optics_layer* out);
OPTICS_API void optics_layer_stack_destroy(optics_layer_stack* stack);
/** @} */

/** @brief Snell propagation angles for each layer (crao in radians). */
OPTICS_API optics_status optics_tmm_propagate_direction(
    const optics_layer_stack* stack,
    optics_complex crao,
    optics_complex* out_angles,
    size_t* out_count);

/** @brief Interface matrices without cumulative multiply (one per interface). */
OPTICS_API optics_status optics_tmm_interface_transfer_matrix_s(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count);

OPTICS_API optics_status optics_tmm_interface_transfer_matrix_p(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count);

/** @brief Interface matrices with thickness phase, cumulative multiply. */
OPTICS_API optics_status optics_tmm_interface_transfer_matrix_with_thickness_s(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count);

OPTICS_API optics_status optics_tmm_interface_transfer_matrix_with_thickness_p(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count);

OPTICS_API optics_status optics_tmm_apply_multilayer_effect(
    optics_matrix2x2* matrices,
    size_t count);

OPTICS_API optics_status optics_tmm_get_structure_pos(
    const optics_layer_stack* stack,
    double* out_pos,
    size_t* out_count);

OPTICS_API optics_status optics_tmm_find_in_structure(
    const double* structure_pos,
    size_t pos_count,
    double depth,
    size_t* out_index,
    double* out_depth_in_structure);

OPTICS_API optics_status optics_tmm_propagate_matrix_to_depth(
    double wavelength,
    const optics_complex* propagate_dir,
    size_t dir_count,
    const optics_layer_stack* stack,
    size_t index,
    double depth_in_structure,
    optics_matrix2x2* out);

OPTICS_API optics_status optics_tmm_get_r_t_from_tmm(
    optics_matrix2x2 tmm,
    optics_complex* out_r,
    optics_complex* out_t);

OPTICS_API optics_status optics_tmm_get_r_t_power_from_tmm_s(
    optics_matrix2x2 tmm,
    optics_complex n1,
    optics_complex theta1,
    optics_complex n2,
    optics_complex theta2,
    double* out_r_power,
    double* out_t_power);

OPTICS_API optics_status optics_tmm_get_r_t_power_from_tmm_p(
    optics_matrix2x2 tmm,
    optics_complex n1,
    optics_complex theta1,
    optics_complex n2,
    optics_complex theta2,
    double* out_r_power,
    double* out_t_power);

OPTICS_API optics_status optics_tmm_get_unpolarized_r_t_power(
    optics_matrix2x2 tmm_s,
    optics_matrix2x2 tmm_p,
    optics_complex n1,
    optics_complex theta1,
    optics_complex n2,
    optics_complex theta2,
    double* out_r_power,
    double* out_t_power);

OPTICS_API optics_status optics_tmm_apply_spr_effect(
    optics_complex nk,
    optics_complex theta,
    optics_complex* out_theta);

OPTICS_API optics_status optics_tmm_get_e_field_s(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_vec3* out);

OPTICS_API optics_status optics_tmm_get_e_field_p(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_vec3* out);

OPTICS_API optics_status optics_tmm_get_poynting_vector_s(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_complex n,
    optics_complex theta0,
    optics_complex n0,
    double* out);

OPTICS_API optics_status optics_tmm_get_poynting_vector_p(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_complex n,
    optics_complex theta0,
    optics_complex n0,
    double* out);

OPTICS_API optics_status optics_tmm_get_absorbed_energy_density_s(
    optics_complex e_forward,
    optics_complex e_backward,
    double wavelength,
    const optics_complex* propagate_dir,
    size_t dir_count,
    const optics_layer_stack* stack,
    size_t index,
    double* out);

OPTICS_API optics_status optics_tmm_get_absorbed_energy_density_p(
    optics_complex e_forward,
    optics_complex e_backward,
    double wavelength,
    const optics_complex* propagate_dir,
    size_t dir_count,
    const optics_layer_stack* stack,
    size_t index,
    double* out);

/**
 * @brief Ellipsometric Psi/Delta (radians) for a stack at oblique incidence.
 * Matches notebook ellips(): atan(|rp/rs|), angle(-rp/rs).
 */
OPTICS_API optics_status optics_tmm_ellipsometric_psi_delta(
    const optics_layer_stack* stack,
    double crao_rad,
    double wavelength,
    double* out_psi,
    double* out_delta);

/**
 * @brief At a depth, compute forward/backward amplitudes and fields (Ex1 helper).
 */
OPTICS_API optics_status optics_tmm_compute_forward_backward_at_depth(
    const optics_layer_stack* stack,
    double crao_rad,
    double wavelength,
    char polarization,
    double depth,
    optics_complex* out_forward,
    optics_complex* out_backward);

#ifdef __cplusplus
}
#endif
