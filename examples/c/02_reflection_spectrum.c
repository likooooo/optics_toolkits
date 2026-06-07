#include "common_example.h"

#include <stdlib.h>

int main(void)
{
    optics_output_init_viz("core_plugins");

    optics_layer_stack* stack = optics_layer_stack_create();
    optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));
    optics_layer_stack_add_layer(stack, OL(2.2, 0.0, 100.0));
    optics_layer_stack_add_layer(stack, OL(3.3, 0.3, 300.0));
    optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));

    const size_t N = 400;
    double kcm[N], Rnorm[N], R45[N];
    const double k_min = 0.0001, k_max = 0.01;
    optics_layer layer0, layer_last;
    optics_layer_stack_get_layer(stack, 0, &layer0);
    optics_layer_stack_get_layer(stack, 3, &layer_last);

    for (size_t i = 0; i < N; ++i) {
        const double k = k_min + (k_max - k_min) * i / (N - 1);
        const double wl = 1.0 / k;
        kcm[i] = k * 1e7;

        optics_complex dir[4];
        size_t n_dir = 4;
        optics_tmm_propagate_direction(stack, (optics_complex){0.0, 0.0}, dir, &n_dir);
        optics_matrix2x2 tmm;
        tmm_last_matrix_s(stack, dir, n_dir, wl, &tmm);
        double rp = 0, tp = 0;
        optics_tmm_get_r_t_power_from_tmm_s(tmm, layer0.nk, dir[0], layer_last.nk, dir[3], &rp, &tp);
        Rnorm[i] = rp;

        optics_tmm_propagate_direction(stack, (optics_complex){M_PI / 4.0, 0.0}, dir, &n_dir);
        optics_matrix2x2 tmm_s, tmm_p;
        tmm_last_matrix_s(stack, dir, n_dir, wl, &tmm_s);
        tmm_last_matrix_p(stack, dir, n_dir, wl, &tmm_p);
        optics_tmm_get_unpolarized_r_t_power(tmm_s, tmm_p, layer0.nk, dir[0], layer_last.nk, dir[3], &rp, &tp);
        R45[i] = rp;
    }

    const char* labels[] = {"Rnorm", "R45"};
    const char* styles[] = {"b-", "m-"};
    const double* ys[] = {Rnorm, R45};
    optics_output_plot_multi_or_csv("output/02_reflection_spectrum.csv", kcm, ys, 2, N, labels, styles,
        "Reflection of unpolarized light at 0 deg (blue), 45 deg (purple)",
        "k (cm^-1)", "Fraction reflected");

    optics_layer_stack_destroy(stack);
    optics_py_shutdown();
    return EXIT_SUCCESS;
}
