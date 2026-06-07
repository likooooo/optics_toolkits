#include "common_example.h"

#include <stdio.h>
#include <stdlib.h>

static void mat_vec(const optics_matrix2x2* M, optics_complex vf, optics_complex vb, optics_complex* of, optics_complex* ob)
{
    *of = (optics_complex){
        M->m[0][0].re * vf.re - M->m[0][0].im * vf.im + M->m[0][1].re * vb.re - M->m[0][1].im * vb.im,
        M->m[0][0].re * vf.im + M->m[0][0].im * vf.re + M->m[0][1].re * vb.im + M->m[0][1].im * vb.re};
    *ob = (optics_complex){
        M->m[1][0].re * vf.re - M->m[1][0].im * vf.im + M->m[1][1].re * vb.re - M->m[1][1].im * vb.im,
        M->m[1][0].re * vf.im + M->m[1][0].im * vf.re + M->m[1][1].re * vb.im + M->m[1][1].im * vb.re};
}

int main(void)
{
    optics_output_init_viz("core_plugins");

    optics_layer_stack* stack = optics_layer_stack_create();
    optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));
    optics_layer_stack_add_layer(stack, OL(2.2, 0.2, 100.0));
    optics_layer_stack_add_layer(stack, OL(3.3, 0.3, 300.0));
    optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));

    const double th0 = M_PI / 4.0;
    const double lambda = 400.0;
    optics_complex dir[4];
    size_t n_dir = 4;
    optics_tmm_propagate_direction(stack, (optics_complex){th0, 0.0}, dir, &n_dir);

    optics_matrix2x2 iface[8];
    size_t n_iface = 8;
    optics_tmm_interface_transfer_matrix_p(stack, dir, n_dir, lambda, iface, &n_iface);

    optics_matrix2x2 applied[8];
    for (size_t i = 0; i < n_iface; ++i)
        applied[i] = iface[i];
    optics_tmm_apply_multilayer_effect(applied, n_iface);
    optics_matrix2x2 tmm_last = applied[n_iface - 1];

    optics_complex r, t;
    optics_tmm_get_r_t_from_tmm(tmm_last, &r, &t);
    optics_complex vw_list[8][2];
    vw_list[n_iface][0] = t;
    vw_list[n_iface][1] = (optics_complex){0.0, 0.0};
    for (int i = (int)n_iface - 1; i >= 0; --i)
        mat_vec(&iface[(size_t)i], vw_list[(size_t)i + 1][0], vw_list[(size_t)i + 1][1],
            &vw_list[(size_t)i][0], &vw_list[(size_t)i][1]);

    double pos[8];
    size_t n_pos = 8;
    optics_tmm_get_structure_pos(stack, pos, &n_pos);

    const size_t N = 240;
    double ds[N], poyn[N], absor[N], ex[N], ey[N], ez[N];
    for (size_t i = 0; i < N; ++i)
        ds[i] = -50.0 + (550.0 * i) / (N - 1);

    for (size_t i = 0; i < N; ++i) {
        size_t layer_id = 0;
        double depth_in = 0.0;
        optics_tmm_find_in_structure(pos, n_pos, ds[i], &layer_id, &depth_in);

        optics_matrix2x2 phase;
        optics_tmm_propagate_matrix_to_depth(lambda, dir, n_dir, stack, layer_id, depth_in, &phase);
        optics_complex vf0 = vw_list[layer_id][0], vb0 = vw_list[layer_id][1];
        optics_complex ef, eb;
        mat_vec(&phase, vf0, vb0, &ef, &eb);

        optics_layer layer;
        optics_layer_stack_get_layer(stack, layer_id, &layer);
        optics_tmm_get_poynting_vector_p(ef, eb, dir[layer_id], layer.nk, dir[0], (optics_complex){1.0, 0.0}, &poyn[i]);
        optics_tmm_get_absorbed_energy_density_p(ef, eb, lambda, dir, n_dir, stack, layer_id, &absor[i]);
        optics_vec3 efield;
        optics_tmm_get_e_field_p(ef, eb, dir[layer_id], &efield);
        ex[i] = efield.x.re;
        ey[i] = efield.y.re;
        ez[i] = efield.z.re;
    }

    double absor_scaled[N];
    for (size_t i = 0; i < N; ++i)
        absor_scaled[i] = 200.0 * absor[i];

    const char* labels1[] = {"Poynting", "200*absorption"};
    const char* styles1[] = {"b-", "m-"};
    const double* y1[] = {poyn, absor_scaled};
    optics_output_plot_multi_or_csv("output/01_poynting_absorption.csv", ds, y1, 2, N, labels1, styles1,
        "Local absorption (purple), Poynting vector (blue)", "depth-z (nm)", "AU");

    const char* labels2[] = {"ex", "ey", "ez"};
    const char* styles2[] = {"r-", "g-", "b-"};
    const double* y2[] = {ex, ey, ez};
    optics_output_plot_multi_or_csv("output/01_e_field.csv", ds, y2, 3, N, labels2, styles2,
        "Polarized-P(TM). ex (red), ey (green), ez (blue)", "depth-z (nm)", "e_field");

    optics_layer_stack_destroy(stack);
    optics_py_shutdown();
    return EXIT_SUCCESS;
}
