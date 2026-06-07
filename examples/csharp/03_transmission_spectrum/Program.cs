using OpticsExamples.Common;
using OpticsToolkits;

var wlTab = new[] { 200.0, 300.0, 400.0, 500.0, 750.0 };
var nkTab = new[]
{
    new OpticsComplex(2.1, 0.1),
    new OpticsComplex(2.4, 0.3),
    new OpticsComplex(2.3, 0.4),
    new OpticsComplex(2.2, 0.4),
    new OpticsComplex(2.2, 0.5)
};
const double depth = 300.0;

const int N = 400;
var lambda = new double[N];
var transmission = new double[N];

for (var i = 0; i < N; i++)
{
    lambda[i] = 200.0 + (550.0 * i) / (N - 1);
    using var stack = new LayerStack();
    stack.AddLayer(1.0, 0.0, 0.0);
    var nk = Tmm.InterpNkQuadratic(wlTab, nkTab, lambda[i]);
    stack.AddLayer(nk.Re, nk.Im, depth);
    stack.AddLayer(1.0, 0.0, 0.0);

    var dir = Tmm.PropagateDirection(stack, 0.0);
    var tmm = Tmm.LastMatrixS(stack, dir, lambda[i]);
    var l0 = stack.GetLayer(0);
    var l2 = stack.GetLayer(2);
    var (_, t) = Tmm.GetRTPowerFromTmmS(tmm, l0.Nk, dir[0], l2.Nk, dir[2]);
    transmission[i] = t;
}

OutputHelper.EnsureOutputDir();
OutputHelper.WriteCsv2Col("output/03_transmission_spectrum.csv",
    "Wavelength (nm)", "Fraction of power transmitted", lambda, transmission);

return 0;
