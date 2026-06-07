namespace OpticsToolkits;

public sealed class LayerStack : IDisposable
{
    private IntPtr _handle;
    private bool _disposed;

    public LayerStack()
    {
        _handle = NativeMethods.optics_layer_stack_create();
        if (_handle == IntPtr.Zero)
            throw new InvalidOperationException("optics_layer_stack_create returned null");
    }

    internal IntPtr Handle
    {
        get
        {
            ObjectDisposedException.ThrowIf(_disposed, this);
            return _handle;
        }
    }

    public void AddLayer(double nkRe, double nkIm, double depth) =>
        AddLayer(new OpticsLayer { Nk = new OpticsComplex(nkRe, nkIm), Depth = depth });

    public void AddLayer(OpticsLayer layer) =>
        NativeMethods.Check(NativeMethods.optics_layer_stack_add_layer(Handle, layer));

    public int LayerCount => (int)NativeMethods.optics_layer_stack_layer_count(Handle);

    public OpticsLayer GetLayer(int index)
    {
        NativeMethods.Check(NativeMethods.optics_layer_stack_get_layer(Handle, (UIntPtr)index, out var layer));
        return layer;
    }

    public void Dispose()
    {
        if (_disposed)
            return;
        if (_handle != IntPtr.Zero)
        {
            NativeMethods.optics_layer_stack_destroy(_handle);
            _handle = IntPtr.Zero;
        }
        _disposed = true;
    }
}
