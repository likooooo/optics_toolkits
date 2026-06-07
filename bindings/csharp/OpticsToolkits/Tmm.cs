namespace OpticsToolkits;

public static class Tmm
{
    public static OpticsComplex[] PropagateDirection(LayerStack stack, double craoRad)
    {
        var angles = new OpticsComplex[16];
        var count = (UIntPtr)angles.Length;
        NativeMethods.Check(NativeMethods.optics_tmm_propagate_direction(
            stack.Handle, new OpticsComplex(craoRad, 0.0), angles, ref count));
        return angles[..(int)count];
    }

    public static OpticsMatrix2x2 LastMatrixS(LayerStack stack, OpticsComplex[] dir, double wavelength)
    {
        var matrices = new OpticsMatrix2x2[16];
        var count = (UIntPtr)matrices.Length;
        NativeMethods.Check(NativeMethods.optics_tmm_interface_transfer_matrix_with_thickness_s(
            stack.Handle, dir, (UIntPtr)dir.Length, wavelength, matrices, ref count));
        return matrices[(int)count - 1];
    }

    public static OpticsMatrix2x2 LastMatrixP(LayerStack stack, OpticsComplex[] dir, double wavelength)
    {
        var matrices = new OpticsMatrix2x2[16];
        var count = (UIntPtr)matrices.Length;
        NativeMethods.Check(NativeMethods.optics_tmm_interface_transfer_matrix_with_thickness_p(
            stack.Handle, dir, (UIntPtr)dir.Length, wavelength, matrices, ref count));
        return matrices[(int)count - 1];
    }

    public static OpticsMatrix2x2[] InterfaceTransferMatrixP(
        LayerStack stack, OpticsComplex[] dir, double wavelength)
    {
        var matrices = new OpticsMatrix2x2[16];
        var count = (UIntPtr)matrices.Length;
        NativeMethods.Check(NativeMethods.optics_tmm_interface_transfer_matrix_p(
            stack.Handle, dir, (UIntPtr)dir.Length, wavelength, matrices, ref count));
        return matrices[..(int)count];
    }

    public static void ApplyMultilayerEffect(OpticsMatrix2x2[] matrices) =>
        NativeMethods.Check(NativeMethods.optics_tmm_apply_multilayer_effect(matrices, (UIntPtr)matrices.Length));

    public static double[] GetStructurePos(LayerStack stack)
    {
        var pos = new double[16];
        var count = (UIntPtr)pos.Length;
        NativeMethods.Check(NativeMethods.optics_tmm_get_structure_pos(stack.Handle, pos, ref count));
        return pos[..(int)count];
    }

    public static (int Index, double DepthInStructure) FindInStructure(double[] structurePos, double depth)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_find_in_structure(
            structurePos, (UIntPtr)structurePos.Length, depth, out var index, out var depthIn));
        return ((int)index, depthIn);
    }

    public static OpticsMatrix2x2 PropagateMatrixToDepth(
        double wavelength, OpticsComplex[] dir, LayerStack stack, int index, double depthInStructure)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_propagate_matrix_to_depth(
            wavelength, dir, (UIntPtr)dir.Length, stack.Handle, (UIntPtr)index, depthInStructure, out var matrix));
        return matrix;
    }

    public static (OpticsComplex R, OpticsComplex T) GetRTFromTmm(OpticsMatrix2x2 tmm)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_r_t_from_tmm(tmm, out var r, out var t));
        return (r, t);
    }

    public static (double RPower, double TPower) GetRTPowerFromTmmS(
        OpticsMatrix2x2 tmm, OpticsComplex n1, OpticsComplex theta1, OpticsComplex n2, OpticsComplex theta2)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_r_t_power_from_tmm_s(
            tmm, n1, theta1, n2, theta2, out var r, out var t));
        return (r, t);
    }

    public static (double RPower, double TPower) GetRTPowerFromTmmP(
        OpticsMatrix2x2 tmm, OpticsComplex n1, OpticsComplex theta1, OpticsComplex n2, OpticsComplex theta2)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_r_t_power_from_tmm_p(
            tmm, n1, theta1, n2, theta2, out var r, out var t));
        return (r, t);
    }

    public static (double RPower, double TPower) GetUnpolarizedRTPower(
        OpticsMatrix2x2 tmmS, OpticsMatrix2x2 tmmP,
        OpticsComplex n1, OpticsComplex theta1, OpticsComplex n2, OpticsComplex theta2)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_unpolarized_r_t_power(
            tmmS, tmmP, n1, theta1, n2, theta2, out var r, out var t));
        return (r, t);
    }

    public static void ApplySprEffect(OpticsComplex nk, ref OpticsComplex theta) =>
        NativeMethods.Check(NativeMethods.optics_tmm_apply_spr_effect(nk, theta, ref theta));

    public static double GetPoyntingVectorP(
        OpticsComplex eForward, OpticsComplex eBackward, OpticsComplex theta,
        OpticsComplex n, OpticsComplex theta0, OpticsComplex n0)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_poynting_vector_p(
            eForward, eBackward, theta, n, theta0, n0, out var poynting));
        return poynting;
    }

    public static double GetAbsorbedEnergyDensityP(
        OpticsComplex eForward, OpticsComplex eBackward, double wavelength,
        OpticsComplex[] dir, LayerStack stack, int index)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_absorbed_energy_density_p(
            eForward, eBackward, wavelength, dir, (UIntPtr)dir.Length, stack.Handle, (UIntPtr)index, out var absorbed));
        return absorbed;
    }

    public static OpticsVec3 GetEFieldP(OpticsComplex eForward, OpticsComplex eBackward, OpticsComplex theta)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_get_e_field_p(eForward, eBackward, theta, out var field));
        return field;
    }

    public static (double Psi, double Delta) EllipsometricPsiDelta(LayerStack stack, double craoRad, double wavelength)
    {
        NativeMethods.Check(NativeMethods.optics_tmm_ellipsometric_psi_delta(
            stack.Handle, craoRad, wavelength, out var psi, out var delta));
        return (psi, delta);
    }

    public static void MatVec(OpticsMatrix2x2 m, OpticsComplex vf, OpticsComplex vb, out OpticsComplex forward, out OpticsComplex backward)
    {
        forward = new OpticsComplex(
            m[0, 0].Re * vf.Re - m[0, 0].Im * vf.Im + m[0, 1].Re * vb.Re - m[0, 1].Im * vb.Im,
            m[0, 0].Re * vf.Im + m[0, 0].Im * vf.Re + m[0, 1].Re * vb.Im + m[0, 1].Im * vb.Re);
        backward = new OpticsComplex(
            m[1, 0].Re * vf.Re - m[1, 0].Im * vf.Im + m[1, 1].Re * vb.Re - m[1, 1].Im * vb.Im,
            m[1, 0].Re * vf.Im + m[1, 0].Im * vf.Re + m[1, 1].Re * vb.Im + m[1, 1].Im * vb.Re);
    }

    public static OpticsComplex InterpNkQuadratic(double[] wl, OpticsComplex[] nk, double x)
    {
        if (wl.Length < 3 || x <= wl[0])
            return nk[0];
        if (x >= wl[^1])
            return nk[^1];

        for (var i = 1; i < wl.Length - 1; i++)
        {
            if (x >= wl[i - 1] && x <= wl[i + 1])
            {
                var x0 = wl[i - 1];
                var x1 = wl[i];
                var x2 = wl[i + 1];
                var l0 = (x - x1) * (x - x2) / ((x0 - x1) * (x0 - x2));
                var l1 = (x - x0) * (x - x2) / ((x1 - x0) * (x1 - x2));
                var l2 = (x - x0) * (x - x1) / ((x2 - x0) * (x2 - x1));
                return new OpticsComplex(
                    l0 * nk[i - 1].Re + l1 * nk[i].Re + l2 * nk[i + 1].Re,
                    l0 * nk[i - 1].Im + l1 * nk[i].Im + l2 * nk[i + 1].Im);
            }
        }

        return nk[wl.Length / 2];
    }
}
