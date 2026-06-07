#include <optics_toolkits/optics_tmm.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static int feq(double a, double b, double tol)
{
    return fabs(a - b) <= tol;
}

static double tol(void)
{
    return (optics_get_precision() == OPTICS_PRECISION_F32) ? 1e-4 : 1e-6;
}

static int test_energy_conservation(void)
{
    optics_layer_stack* stack = optics_layer_stack_create();
    optics_layer layers[] = {
        {{1.0, 0.0}, 0.0},
        {{2.2, 0.0}, 100.0},
        {{1.0, 0.0}, 0.0},
    };
    for (size_t i = 0; i < 3; ++i) {
        if (optics_layer_stack_add_layer(stack, layers[i]) != OPTICS_OK)
            return 1;
    }

    optics_complex angles[3];
    size_t n_angles = 3;
    if (optics_tmm_propagate_direction(stack, (optics_complex){0.0, 0.0}, angles, &n_angles) != OPTICS_OK)
        return 1;

    optics_matrix2x2 tmm_buf[8];
    size_t n_m = sizeof(tmm_buf) / sizeof(tmm_buf[0]);
    if (optics_tmm_interface_transfer_matrix_with_thickness_s(stack, angles, n_angles, 500.0, tmm_buf, &n_m) != OPTICS_OK) {
        fprintf(stderr, "tmm failed: %s\n", optics_last_error());
        return 1;
    }
    const optics_matrix2x2 tmm = tmm_buf[n_m - 1];

    double r_power = 0.0, t_power = 0.0;
    if (optics_tmm_get_r_t_power_from_tmm_s(
            tmm,
            layers[0].nk, angles[0],
            layers[2].nk, angles[2],
            &r_power, &t_power) != OPTICS_OK)
        return 1;

    optics_layer_stack_destroy(stack);

    if (!feq(r_power + t_power, 1.0, tol())) {
        fprintf(stderr, "energy conservation failed: R+T=%g\n", r_power + t_power);
        return 1;
    }
    return 0;
}

static int test_ellipsometry(void)
{
    /* Ex4: air/SiO2/Si @ 70 deg, 633 nm — spot-check handbook-style curve. */
    struct { double d_nm; double psi_deg; double delta_deg; } refs[] = {
        {0.0, 10.48490138316374, 0.8210210726866115},
        {500.0, 27.880169220887296, -93.583141432460465},
    };
    const double crao = 70.0 * 3.14159265358979323846 / 180.0;
    const double wavelength = 633.0;
    const double ang_tol = (optics_get_precision() == OPTICS_PRECISION_F32) ? 0.5 : 0.15;

    for (size_t r = 0; r < sizeof(refs) / sizeof(refs[0]); ++r) {
        optics_layer_stack* stack = optics_layer_stack_create();
        optics_layer_stack_add_layer(stack, (optics_layer){{1.0, 0.0}, 0.0});
        optics_layer_stack_add_layer(stack, (optics_layer){{1.46, 0.0}, refs[r].d_nm});
        optics_layer_stack_add_layer(stack, (optics_layer){{3.87, 0.02}, 0.0});

        double psi = 0.0, delta = 0.0;
        if (optics_tmm_ellipsometric_psi_delta(stack, crao, wavelength, &psi, &delta) != OPTICS_OK) {
            fprintf(stderr, "ellipsometry failed: %s\n", optics_last_error());
            optics_layer_stack_destroy(stack);
            return 1;
        }
        optics_layer_stack_destroy(stack);

        const double psi_deg = psi * 180.0 / 3.14159265358979323846;
        const double delta_deg = delta * 180.0 / 3.14159265358979323846;
        if (!feq(psi_deg, refs[r].psi_deg, ang_tol) || !feq(delta_deg, refs[r].delta_deg, ang_tol)) {
            fprintf(stderr, "ellipsometry mismatch d=%g: psi=%g delta=%g (expected %g %g)\n",
                refs[r].d_nm, psi_deg, delta_deg, refs[r].psi_deg, refs[r].delta_deg);
            return 1;
        }
    }
    return 0;
}

static int test_spr_energy_conservation(void)
{
    /* Ex5: absorbing metal stack — power fractions valid; R+T <= 1; SPR dip present. */
    optics_layer_stack* stack = optics_layer_stack_create();
    optics_layer_stack_add_layer(stack, (optics_layer){{1.517, 0.0}, 0.0});
    optics_layer_stack_add_layer(stack, (optics_layer){{3.719, 4.362}, 5.0});
    optics_layer_stack_add_layer(stack, (optics_layer){{0.130, 3.162}, 30.0});
    optics_layer_stack_add_layer(stack, (optics_layer){{1.0, 0.0}, 0.0});

    optics_layer l0, l3;
    optics_layer_stack_get_layer(stack, 0, &l0);
    optics_layer_stack_get_layer(stack, 3, &l3);

    const double lambda = 633.0;
    double min_rp = 1.0;
    for (int i = 0; i < 20; ++i) {
        const double theta = (30.0 + 1.5 * i) * 3.14159265358979323846 / 180.0;
        optics_complex dir[4];
        size_t n_dir = 4;
        if (optics_tmm_propagate_direction(stack, (optics_complex){theta, 0.0}, dir, &n_dir) != OPTICS_OK)
            return 1;
        if (optics_tmm_apply_spr_effect(l0.nk, dir[0], &dir[0]) != OPTICS_OK)
            return 1;
        if (optics_tmm_apply_spr_effect(l3.nk, dir[3], &dir[3]) != OPTICS_OK)
            return 1;

        optics_matrix2x2 buf[8];
        size_t n_m = sizeof(buf) / sizeof(buf[0]);
        if (optics_tmm_interface_transfer_matrix_with_thickness_p(stack, dir, n_dir, lambda, buf, &n_m) != OPTICS_OK)
            return 1;

        double rp = 0.0, tp = 0.0;
        if (optics_tmm_get_r_t_power_from_tmm_p(buf[n_m - 1], l0.nk, dir[0], l3.nk, dir[3], &rp, &tp) != OPTICS_OK)
            return 1;

        if (rp < -tol() || tp < -tol() || rp > 1.0 + tol() || tp > 1.0 + tol() || rp + tp > 1.0 + tol()) {
            fprintf(stderr, "SPR power bounds failed at step %d: R=%g T=%g\n", i, rp, tp);
            optics_layer_stack_destroy(stack);
            return 1;
        }
        if (rp < min_rp)
            min_rp = rp;
    }

    optics_layer_stack_destroy(stack);

    if (min_rp > 0.05) {
        fprintf(stderr, "SPR dip not observed: min Rp=%g\n", min_rp);
        return 1;
    }
    return 0;
}

int main(void)
{
    printf("optics_toolkits %s api=%d precision=%d\n",
        optics_get_version(),
        optics_get_api_version(),
        (int)optics_get_precision());

    if (test_energy_conservation() != 0)
        return EXIT_FAILURE;
    if (test_ellipsometry() != 0)
        return EXIT_FAILURE;
    if (test_spr_energy_conservation() != 0)
        return EXIT_FAILURE;

    printf("test_tmm passed\n");
    return EXIT_SUCCESS;
}
