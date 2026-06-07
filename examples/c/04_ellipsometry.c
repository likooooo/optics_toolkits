#include "common_example.h"

#include <stdlib.h>

int main(void)
{
    optics_output_init_viz("core_plugins");

    optics_layer_stack* stack = optics_layer_stack_create();

    const size_t N = 100;
    double ds[N], psis[N], deltas[N];
    const double crao = 70.0 * M_PI / 180.0;
    const double wavelength = 633.0;

    for (size_t i = 0; i < N; ++i) {
        ds[i] = (1000.0 * i) / (N - 1);
        optics_layer_stack_destroy(stack);
        stack = optics_layer_stack_create();
        optics_layer_stack_add_layer(stack, OL(1.0, 0.0, 0.0));
        optics_layer_stack_add_layer(stack, OL(1.46, 0.0, ds[i]));
        optics_layer_stack_add_layer(stack, OL(3.87, 0.02, 0.0));
        double psi = 0, delta = 0;
        optics_tmm_ellipsometric_psi_delta(stack, crao, wavelength, &psi, &delta);
        psis[i] = psi * 180.0 / M_PI;
        deltas[i] = delta * 180.0 / M_PI;
    }

    const char* labels[] = {"Psi", "Delta"};
    const char* styles[] = {"b-", "r-"};
    const double* ys[] = {psis, deltas};
    optics_output_plot_multi_or_csv("output/04_ellipsometry.csv", ds, ys, 2, N, labels, styles,
        "Ellipsometric parameters for air/SiO2/Si @ 70 deg, 633nm",
        "SiO2 thickness (nm)", "Ellipsometric angles (degrees)");

    optics_layer_stack_destroy(stack);
    optics_py_shutdown();
    return EXIT_SUCCESS;
}
