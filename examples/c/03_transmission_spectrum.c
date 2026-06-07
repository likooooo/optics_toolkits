#include "common_example.h"

#include <stdlib.h>

int main(void)
{
    optics_output_init_viz("core_plugins");

    const double wl_tab[] = {200, 300, 400, 500, 750};
    const optics_complex nk_tab[] = {
        {2.1, 0.1}, {2.4, 0.3}, {2.3, 0.4}, {2.2, 0.4}, {2.2, 0.5}};
    const size_t n_tab = 5;
    const double depth = 300.0;

    const size_t N = 400;
    double lambda[N], T[N];
    for (size_t i = 0; i < N; ++i) {
        lambda[i] = 200.0 + (550.0 * i) / (N - 1);
        optics_layer_stack* stack = optics_layer_stack_create();
        optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));
        optics_complex nk = interp_nk_quadratic(wl_tab, nk_tab, n_tab, lambda[i]);
        optics_layer_stack_add_layer(stack, (optics_layer){nk, depth});
        optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));

        optics_complex dir[3];
        size_t n_dir = 3;
        optics_tmm_propagate_direction(stack, (optics_complex){0.0, 0.0}, dir, &n_dir);
        optics_matrix2x2 tmm;
        tmm_last_matrix_s(stack, dir, n_dir, lambda[i], &tmm);
        optics_layer l0, l2;
        optics_layer_stack_get_layer(stack, 0, &l0);
        optics_layer_stack_get_layer(stack, 2, &l2);
        double r = 0, t = 0;
        optics_tmm_get_r_t_power_from_tmm_s(tmm, l0.nk, dir[0], l2.nk, dir[2], &r, &t);
        T[i] = t;
        optics_layer_stack_destroy(stack);
    }

    optics_output_plot_xy_or_csv("output/03_transmission_spectrum.csv", lambda, T, N,
        "Transmission at normal incidence", "Wavelength (nm)", "Fraction of power transmitted", "b-");

    optics_py_shutdown();
    return EXIT_SUCCESS;
}
