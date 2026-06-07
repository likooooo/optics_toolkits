using OpticsExamples.Common;
using OpticsToolkits;

using var stack = new LayerStack();
stack.AddLayer(1.0, 0.0, 0.0);
stack.AddLayer(2.2, 0.0, 100.0);
stack.AddLayer(3.3, 0.3, 300.0);
stack.AddLayer(1.0, 0.0, 0.0);

const int N = 400;
const double kMin = 0.0001;
const double kMax = 0.01;
var kcm = new double[N];
var rNorm = new double[N];
var r45 = new double[N];

var layer0 = stack.GetLayer(0);
var layerLast = stack.GetLayer(3);

for (var i = 0; i < N; i++)
{
    var k = kMin + (kMax - kMin) * i / (N - 1);
    var wl = 1.0 / k;
    kcm[i] = k * 1e7;

    var dir = Tmm.PropagateDirection(stack, 0.0);
    var tmm = Tmm.LastMatrixS(stack, dir, wl);
    var (rp, _) = Tmm.GetRTPowerFromTmmS(tmm, layer0.Nk, dir[0], layerLast.Nk, dir[3]);
    rNorm[i] = rp;

    dir = Tmm.PropagateDirection(stack, Math.PI / 4.0);
    var tmmS = Tmm.LastMatrixS(stack, dir, wl);
    var tmmP = Tmm.LastMatrixP(stack, dir, wl);
    (rp, _) = Tmm.GetUnpolarizedRTPower(tmmS, tmmP, layer0.Nk, dir[0], layerLast.Nk, dir[3]);
    r45[i] = rp;
}

OutputHelper.EnsureOutputDir();
OutputHelper.WriteCsvMulti("output/02_reflection_spectrum.csv", "k (cm^-1)",
    ["Rnorm", "R45"], kcm, [rNorm, r45]);

return 0;
