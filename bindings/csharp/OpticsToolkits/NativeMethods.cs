using System.Reflection;
using System.Runtime.InteropServices;

namespace OpticsToolkits;

[StructLayout(LayoutKind.Sequential)]
public struct OpticsComplex
{
    public double Re;
    public double Im;

    public OpticsComplex(double re, double im)
    {
        Re = re;
        Im = im;
    }
}

[StructLayout(LayoutKind.Sequential)]
public struct OpticsLayer
{
    public OpticsComplex Nk;
    public double Depth;
}

[StructLayout(LayoutKind.Sequential)]
public struct OpticsMatrix2x2
{
    public OpticsComplex M00;
    public OpticsComplex M01;
    public OpticsComplex M10;
    public OpticsComplex M11;

    public OpticsComplex this[int row, int col] => (row, col) switch
    {
        (0, 0) => M00,
        (0, 1) => M01,
        (1, 0) => M10,
        (1, 1) => M11,
        _ => throw new IndexOutOfRangeException()
    };
}

[StructLayout(LayoutKind.Sequential)]
public struct OpticsVec3
{
    public OpticsComplex X;
    public OpticsComplex Y;
    public OpticsComplex Z;
}

public enum OpticsPrecisionKind
{
    F32 = 0,
    F64 = 1
}

internal static partial class NativeMethods
{
    public const string DllName = "optics_toolkits";

    static NativeMethods()
    {
        NativeLibrary.SetDllImportResolver(typeof(NativeMethods).Assembly, ResolveLibrary);
    }

    private static IntPtr ResolveLibrary(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
    {
        if (libraryName != DllName)
            return IntPtr.Zero;

        var libPath = Environment.GetEnvironmentVariable("OPTICS_TOOLKITS_LIB_PATH");
        if (!string.IsNullOrEmpty(libPath))
        {
            var candidate = Path.Combine(libPath, GetPlatformLibraryName(DllName));
            if (NativeLibrary.TryLoad(candidate, out var handle))
                return handle;
        }

        if (NativeLibrary.TryLoad(DllName, assembly, searchPath, out var loaded))
            return loaded;

        return IntPtr.Zero;
    }

    private static string GetPlatformLibraryName(string baseName) =>
        OperatingSystem.IsWindows() ? $"{baseName}.dll" :
        OperatingSystem.IsMacOS() ? $"lib{baseName}.dylib" :
        $"lib{baseName}.so";

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr optics_get_version();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int optics_get_api_version();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsPrecisionKind optics_get_precision();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern int optics_has_python_core_plugins();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr optics_last_error();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void optics_clear_error();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr optics_layer_stack_create();

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_layer_stack_add_layer(IntPtr stack, OpticsLayer layer);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern UIntPtr optics_layer_stack_layer_count(IntPtr stack);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_layer_stack_get_layer(IntPtr stack, UIntPtr index, out OpticsLayer layer);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern void optics_layer_stack_destroy(IntPtr stack);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_propagate_direction(
        IntPtr stack, OpticsComplex crao, [Out] OpticsComplex[] outAngles, ref UIntPtr outCount);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_interface_transfer_matrix_s(
        IntPtr stack, OpticsComplex[] propagateDir, UIntPtr dirCount, double wavelength,
        [Out] OpticsMatrix2x2[] outMatrices, ref UIntPtr outCount);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_interface_transfer_matrix_p(
        IntPtr stack, OpticsComplex[] propagateDir, UIntPtr dirCount, double wavelength,
        [Out] OpticsMatrix2x2[] outMatrices, ref UIntPtr outCount);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_interface_transfer_matrix_with_thickness_s(
        IntPtr stack, OpticsComplex[] propagateDir, UIntPtr dirCount, double wavelength,
        [Out] OpticsMatrix2x2[] outMatrices, ref UIntPtr outCount);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_interface_transfer_matrix_with_thickness_p(
        IntPtr stack, OpticsComplex[] propagateDir, UIntPtr dirCount, double wavelength,
        [Out] OpticsMatrix2x2[] outMatrices, ref UIntPtr outCount);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_apply_multilayer_effect(OpticsMatrix2x2[] matrices, UIntPtr count);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_structure_pos(
        IntPtr stack, [Out] double[] outPos, ref UIntPtr outCount);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_find_in_structure(
        double[] structurePos, UIntPtr posCount, double depth,
        out UIntPtr outIndex, out double outDepthInStructure);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_propagate_matrix_to_depth(
        double wavelength, OpticsComplex[] propagateDir, UIntPtr dirCount,
        IntPtr stack, UIntPtr index, double depthInStructure, out OpticsMatrix2x2 matrix);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_r_t_from_tmm(
        OpticsMatrix2x2 tmm, out OpticsComplex r, out OpticsComplex t);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_r_t_power_from_tmm_s(
        OpticsMatrix2x2 tmm, OpticsComplex n1, OpticsComplex theta1,
        OpticsComplex n2, OpticsComplex theta2, out double rPower, out double tPower);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_r_t_power_from_tmm_p(
        OpticsMatrix2x2 tmm, OpticsComplex n1, OpticsComplex theta1,
        OpticsComplex n2, OpticsComplex theta2, out double rPower, out double tPower);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_unpolarized_r_t_power(
        OpticsMatrix2x2 tmmS, OpticsMatrix2x2 tmmP,
        OpticsComplex n1, OpticsComplex theta1, OpticsComplex n2, OpticsComplex theta2,
        out double rPower, out double tPower);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_apply_spr_effect(
        OpticsComplex nk, OpticsComplex theta, ref OpticsComplex outTheta);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_e_field_s(
        OpticsComplex eForward, OpticsComplex eBackward, OpticsComplex theta, out OpticsVec3 field);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_e_field_p(
        OpticsComplex eForward, OpticsComplex eBackward, OpticsComplex theta, out OpticsVec3 field);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_poynting_vector_s(
        OpticsComplex eForward, OpticsComplex eBackward, OpticsComplex theta,
        OpticsComplex n, OpticsComplex theta0, OpticsComplex n0, out double poynting);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_poynting_vector_p(
        OpticsComplex eForward, OpticsComplex eBackward, OpticsComplex theta,
        OpticsComplex n, OpticsComplex theta0, OpticsComplex n0, out double poynting);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_absorbed_energy_density_s(
        OpticsComplex eForward, OpticsComplex eBackward, double wavelength,
        OpticsComplex[] propagateDir, UIntPtr dirCount, IntPtr stack, UIntPtr index, out double absorbed);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_get_absorbed_energy_density_p(
        OpticsComplex eForward, OpticsComplex eBackward, double wavelength,
        OpticsComplex[] propagateDir, UIntPtr dirCount, IntPtr stack, UIntPtr index, out double absorbed);

    [DllImport(DllName, CallingConvention = CallingConvention.Cdecl)]
    public static extern OpticsStatus optics_tmm_ellipsometric_psi_delta(
        IntPtr stack, double craoRad, double wavelength, out double psi, out double delta);

    public static void Check(OpticsStatus status)
    {
        if (status != OpticsStatus.Ok)
        {
            var msg = Marshal.PtrToStringUTF8(optics_last_error()) ?? status.ToString();
            throw new InvalidOperationException($"optics_toolkits error ({status}): {msg}");
        }
    }
}
