#include <mex.h>
#include <stdint.h>
#include <string.h>

#include <optics_toolkits/optics_tmm.h>

static optics_layer_stack* ptr_from_mx(const mxArray* a)
{
    return (optics_layer_stack*)(uintptr_t)mxGetScalar(a);
}

static mxArray* ptr_to_mx(optics_layer_stack* p)
{
    return mxCreateDoubleScalar((double)(uintptr_t)p);
}

static optics_complex complex_from_mx(const mxArray* a)
{
    optics_complex c = {0.0, 0.0};
    if (mxIsComplex(a)) {
        c.re = mxGetPr(a)[0];
        c.im = mxGetPi(a)[0];
    } else {
        c.re = mxGetScalar(a);
    }
    return c;
}

static mxArray* complex_to_mx(optics_complex c)
{
    mxArray* out = mxCreateDoubleMatrix(1, 1, mxCOMPLEX);
    *mxGetPr(out) = c.re;
    *mxGetPi(out) = c.im;
    return out;
}

static void fail_status(optics_status st)
{
    if (st != OPTICS_OK) {
        const char* msg = optics_last_error();
        mexErrMsgIdAndTxt("optics_toolkits:error", "%s", msg ? msg : "unknown error");
    }
}

static mxArray* matrix_to_mx(const optics_matrix2x2* m)
{
    mxArray* out = mxCreateDoubleMatrix(2, 2, mxCOMPLEX);
    double* pr = mxGetPr(out);
    double* pi = mxGetPi(out);
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            const size_t idx = (size_t)r + (size_t)c * 2;
            pr[idx] = m->m[r][c].re;
            pi[idx] = m->m[r][c].im;
        }
    }
    return out;
}

static optics_matrix2x2 matrix_from_mx(const mxArray* a)
{
    optics_matrix2x2 m = {{{{0}}}};
    const double* pr = mxGetPr(a);
    const double* pi = mxIsComplex(a) ? mxGetPi(a) : NULL;
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            const size_t idx = (size_t)r + (size_t)c * 2;
            m.m[r][c].re = pr[idx];
            m.m[r][c].im = pi ? pi[idx] : 0.0;
        }
    }
    return m;
}

static mxArray* angles_to_mx(const optics_complex* angles, size_t n)
{
    mxArray* out = mxCreateDoubleMatrix(1, (mwSize)n, mxCOMPLEX);
    double* pr = mxGetPr(out);
    double* pi = mxGetPi(out);
    for (size_t i = 0; i < n; ++i) {
        pr[i] = angles[i].re;
        pi[i] = angles[i].im;
    }
    return out;
}

static size_t angles_from_mx(const mxArray* a, optics_complex* out, size_t cap)
{
    const size_t n = (size_t)mxGetNumberOfElements(a);
    if (n > cap)
        mexErrMsgIdAndTxt("optics_toolkits:arg", "angle buffer too small");
    const double* pr = mxGetPr(a);
    const double* pi = mxIsComplex(a) ? mxGetPi(a) : NULL;
    for (size_t i = 0; i < n; ++i) {
        out[i].re = pr[i];
        out[i].im = pi ? pi[i] : 0.0;
    }
    return n;
}

void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray* prhs[])
{
    if (nrhs < 1 || !mxIsChar(prhs[0]))
        mexErrMsgIdAndTxt("optics_toolkits:arg", "first argument must be command string");

    char cmd[64] = {0};
    if (mxGetString(prhs[0], cmd, sizeof(cmd)))
        mexErrMsgIdAndTxt("optics_toolkits:arg", "failed to read command");

    if (strcmp(cmd, "version") == 0) {
        plhs[0] = mxCreateString(optics_get_version());
        return;
    }

    if (strcmp(cmd, "create") == 0) {
        optics_layer_stack* stack = optics_layer_stack_create();
        if (!stack)
            mexErrMsgIdAndTxt("optics_toolkits:error", "optics_layer_stack_create failed");
        plhs[0] = ptr_to_mx(stack);
        return;
    }

    if (strcmp(cmd, "destroy") == 0) {
        if (nrhs < 2)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "destroy requires handle");
        optics_layer_stack_destroy(ptr_from_mx(prhs[1]));
        return;
    }

    if (strcmp(cmd, "add_layer") == 0) {
        if (nrhs < 5)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "add_layer(handle, nk_re, nk_im, depth)");
        optics_layer layer = {
            {mxGetScalar(prhs[2]), mxGetScalar(prhs[3])},
            mxGetScalar(prhs[4])};
        fail_status(optics_layer_stack_add_layer(ptr_from_mx(prhs[1]), layer));
        return;
    }

    if (strcmp(cmd, "propagate_direction") == 0) {
        if (nrhs < 3)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "propagate_direction(handle, crao_rad)");
        optics_complex angles[16];
        size_t n = 16;
        fail_status(optics_tmm_propagate_direction(
            ptr_from_mx(prhs[1]), complex_from_mx(prhs[2]), angles, &n));
        plhs[0] = angles_to_mx(angles, n);
        return;
    }

    if (strcmp(cmd, "last_matrix_s") == 0 || strcmp(cmd, "last_matrix_p") == 0) {
        if (nrhs < 4)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "last_matrix_*(handle, dir, wavelength)");
        optics_complex dir[16];
        const size_t n_dir = angles_from_mx(prhs[2], dir, 16);
        optics_matrix2x2 buf[16];
        size_t n = 16;
        const double wl = mxGetScalar(prhs[3]);
        optics_status st = (strcmp(cmd, "last_matrix_s") == 0)
            ? optics_tmm_interface_transfer_matrix_with_thickness_s(
                  ptr_from_mx(prhs[1]), dir, n_dir, wl, buf, &n)
            : optics_tmm_interface_transfer_matrix_with_thickness_p(
                  ptr_from_mx(prhs[1]), dir, n_dir, wl, buf, &n);
        fail_status(st);
        plhs[0] = matrix_to_mx(&buf[n - 1]);
        return;
    }

    if (strcmp(cmd, "interface_transfer_matrix_p") == 0) {
        if (nrhs < 4)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "interface_transfer_matrix_p(handle, dir, wavelength)");
        optics_complex dir[16];
        const size_t n_dir = angles_from_mx(prhs[2], dir, 16);
        optics_matrix2x2 buf[16];
        size_t n = 16;
        fail_status(optics_tmm_interface_transfer_matrix_p(
            ptr_from_mx(prhs[1]), dir, n_dir, mxGetScalar(prhs[3]), buf, &n));
        plhs[0] = mxCreateCellMatrix(1, (mwSize)n);
        for (size_t i = 0; i < n; ++i)
            mxSetCell(plhs[0], (mwIndex)i, matrix_to_mx(&buf[i]));
        return;
    }

    if (strcmp(cmd, "apply_multilayer_effect") == 0) {
        if (nrhs < 2 || !mxIsCell(prhs[1]))
            mexErrMsgIdAndTxt("optics_toolkits:arg", "apply_multilayer_effect(cell of 2x2)");
        const mwSize n = mxGetNumberOfElements(prhs[1]);
        optics_matrix2x2* mats = (optics_matrix2x2*)mxMalloc(n * sizeof(optics_matrix2x2));
        for (mwIndex i = 0; i < n; ++i)
            mats[i] = matrix_from_mx(mxGetCell(prhs[1], i));
        fail_status(optics_tmm_apply_multilayer_effect(mats, (size_t)n));
        plhs[0] = mxCreateCellMatrix(1, n);
        for (mwIndex i = 0; i < n; ++i)
            mxSetCell(plhs[0], i, matrix_to_mx(&mats[i]));
        mxFree(mats);
        return;
    }

    if (strcmp(cmd, "get_structure_pos") == 0) {
        double pos[16];
        size_t n = 16;
        fail_status(optics_tmm_get_structure_pos(ptr_from_mx(prhs[1]), pos, &n));
        plhs[0] = mxCreateDoubleMatrix(1, (mwSize)n, mxREAL);
        memcpy(mxGetPr(plhs[0]), pos, n * sizeof(double));
        return;
    }

    if (strcmp(cmd, "find_in_structure") == 0) {
        if (nrhs < 3)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "find_in_structure(pos, depth)");
        const double* pos = mxGetPr(prhs[1]);
        const size_t n = (size_t)mxGetNumberOfElements(prhs[1]);
        size_t idx = 0;
        double depth_in = 0.0;
        fail_status(optics_tmm_find_in_structure(pos, n, mxGetScalar(prhs[2]), &idx, &depth_in));
        plhs[0] = mxCreateDoubleScalar((double)idx);
        if (nlhs > 1)
            plhs[1] = mxCreateDoubleScalar(depth_in);
        return;
    }

    if (strcmp(cmd, "propagate_matrix_to_depth") == 0) {
        if (nrhs < 6)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "propagate_matrix_to_depth(...)");
        optics_complex dir[16];
        const size_t n_dir = angles_from_mx(prhs[2], dir, 16);
        optics_matrix2x2 out;
        fail_status(optics_tmm_propagate_matrix_to_depth(
            mxGetScalar(prhs[3]), dir, n_dir, ptr_from_mx(prhs[1]),
            (size_t)mxGetScalar(prhs[4]), mxGetScalar(prhs[5]), &out));
        plhs[0] = matrix_to_mx(&out);
        return;
    }

    if (strcmp(cmd, "get_r_t_from_tmm") == 0) {
        optics_complex r, t;
        fail_status(optics_tmm_get_r_t_from_tmm(matrix_from_mx(prhs[1]), &r, &t));
        plhs[0] = complex_to_mx(r);
        if (nlhs > 1)
            plhs[1] = complex_to_mx(t);
        return;
    }

    if (strcmp(cmd, "get_r_t_power_s") == 0 || strcmp(cmd, "get_r_t_power_p") == 0) {
        if (nrhs < 6)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "get_r_t_power_*(tmm, n1, th1, n2, th2)");
        double rp = 0, tp = 0;
        optics_status st = (strcmp(cmd, "get_r_t_power_s") == 0)
            ? optics_tmm_get_r_t_power_from_tmm_s(matrix_from_mx(prhs[1]),
                  complex_from_mx(prhs[2]), complex_from_mx(prhs[3]),
                  complex_from_mx(prhs[4]), complex_from_mx(prhs[5]), &rp, &tp)
            : optics_tmm_get_r_t_power_from_tmm_p(matrix_from_mx(prhs[1]),
                  complex_from_mx(prhs[2]), complex_from_mx(prhs[3]),
                  complex_from_mx(prhs[4]), complex_from_mx(prhs[5]), &rp, &tp);
        fail_status(st);
        plhs[0] = mxCreateDoubleScalar(rp);
        if (nlhs > 1)
            plhs[1] = mxCreateDoubleScalar(tp);
        return;
    }

    if (strcmp(cmd, "get_unpolarized_r_t_power") == 0) {
        if (nrhs < 7)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "get_unpolarized_r_t_power(tmm_s, tmm_p, ...)");
        double rp = 0, tp = 0;
        fail_status(optics_tmm_get_unpolarized_r_t_power(
            matrix_from_mx(prhs[1]), matrix_from_mx(prhs[2]),
            complex_from_mx(prhs[3]), complex_from_mx(prhs[4]),
            complex_from_mx(prhs[5]), complex_from_mx(prhs[6]), &rp, &tp));
        plhs[0] = mxCreateDoubleScalar(rp);
        if (nlhs > 1)
            plhs[1] = mxCreateDoubleScalar(tp);
        return;
    }

    if (strcmp(cmd, "apply_spr_effect") == 0) {
        optics_complex theta = complex_from_mx(prhs[2]);
        fail_status(optics_tmm_apply_spr_effect(complex_from_mx(prhs[1]), theta, &theta));
        plhs[0] = complex_to_mx(theta);
        return;
    }

    if (strcmp(cmd, "get_poynting_vector_p") == 0) {
        if (nrhs < 7)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "get_poynting_vector_p(...)");
        double out = 0;
        fail_status(optics_tmm_get_poynting_vector_p(
            complex_from_mx(prhs[1]), complex_from_mx(prhs[2]), complex_from_mx(prhs[3]),
            complex_from_mx(prhs[4]), complex_from_mx(prhs[5]), complex_from_mx(prhs[6]), &out));
        plhs[0] = mxCreateDoubleScalar(out);
        return;
    }

    if (strcmp(cmd, "get_absorbed_energy_density_p") == 0) {
        if (nrhs < 7)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "get_absorbed_energy_density_p(...)");
        optics_complex dir[16];
        const size_t n_dir = angles_from_mx(prhs[4], dir, 16);
        double out = 0;
        fail_status(optics_tmm_get_absorbed_energy_density_p(
            complex_from_mx(prhs[1]), complex_from_mx(prhs[2]), mxGetScalar(prhs[3]),
            dir, n_dir, ptr_from_mx(prhs[5]), (size_t)mxGetScalar(prhs[6]), &out));
        plhs[0] = mxCreateDoubleScalar(out);
        return;
    }

    if (strcmp(cmd, "ellipsometric_psi_delta") == 0) {
        if (nrhs < 4)
            mexErrMsgIdAndTxt("optics_toolkits:arg", "ellipsometric_psi_delta(handle, crao, wl)");
        double psi = 0, delta = 0;
        fail_status(optics_tmm_ellipsometric_psi_delta(
            ptr_from_mx(prhs[1]), mxGetScalar(prhs[2]), mxGetScalar(prhs[3]), &psi, &delta));
        plhs[0] = mxCreateDoubleScalar(psi);
        if (nlhs > 1)
            plhs[1] = mxCreateDoubleScalar(delta);
        return;
    }

    mexErrMsgIdAndTxt("optics_toolkits:arg", "unknown command: %s", cmd);
}
