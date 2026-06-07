#include "common_example.h"

#include <stdlib.h>

int main(void)
{
    optics_output_init_viz("core_plugins");

    optics_layer_stack* stack = optics_layer_stack_create();
    optics_layer_stack_add_layer(stack, OL(1.517, 0.0, 0.0));
    optics_layer_stack_add_layer(stack, OL(3.719, 4.362, 5.0));
    optics_layer_stack_add_layer(stack, OL(0.130, 3.162, 30.0));
    optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));

    optics_layer l0, l3;
    optics_layer_stack_get_layer(stack, 0, &l0);
    optics_layer_stack_get_layer(stack, 3, &l3);

    const double lambda = 633.0;
    const size_t N = 300;
    double theta_deg[N], Rp[N], Tp[N];
    for (size_t i = 0; i < N; ++i) {
        const double theta = (30.0 + (30.0 * i) / (N - 1)) * M_PI / 180.0;
        theta_deg[i] = theta * 180.0 / M_PI;
        optics_complex dir[4];
        size_t n_dir = 4;
        optics_tmm_propagate_direction(stack, (optics_complex){theta, 0.0}, dir, &n_dir);
        optics_tmm_apply_spr_effect(l0.nk, dir[0], &dir[0]);
        optics_tmm_apply_spr_effect(l3.nk, dir[3], &dir[3]);
        optics_matrix2x2 tmm;
        tmm_last_matrix_p(stack, dir, n_dir, lambda, &tmm);
        optics_tmm_get_r_t_power_from_tmm_p(tmm, l0.nk, dir[0], l3.nk, dir[3], &Rp[i], &Tp[i]);
    }

    const char* labels[] = {"Rp", "Tp"};
    const char* styles[] = {"b-", "r-"};
    const double* ys[] = {Rp, Tp};
    optics_output_plot_multi_or_csv("output/05_surface_plasmon_resonance.csv", theta_deg, ys, 2, N, labels, styles,
        "Reflection of p-polarized light with Surface Plasmon Resonance",
        "theta (degree)", "Fraction reflected/transmitted");

    optics_layer_stack_destroy(stack);
    optics_py_shutdown();
    return EXIT_SUCCESS;
}
