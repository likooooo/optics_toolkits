using OpticsExamples.Common;
using OpticsToolkits;

const double crao = 70.0 * Math.PI / 180.0;
const double wavelength = 633.0;
const int N = 100;

var ds = new double[N];
var psis = new double[N];
var deltas = new double[N];

for (var i = 0; i < N; i++)
{
    ds[i] = (1000.0 * i) / (N - 1);
    using var stack = new LayerStack();
    stack.AddLayer(1.0, 0.0, 0.0);
    stack.AddLayer(1.46, 0.0, ds[i]);
    stack.AddLayer(3.87, 0.02, 0.0);
    var (psi, delta) = Tmm.EllipsometricPsiDelta(stack, crao, wavelength);
    psis[i] = psi * 180.0 / Math.PI;
    deltas[i] = delta * 180.0 / Math.PI;
}

OutputHelper.EnsureOutputDir();
OutputHelper.WriteCsvMulti("output/04_ellipsometry.csv", "SiO2 thickness (nm)",
    ["Psi", "Delta"], ds, [psis, deltas]);

return 0;
