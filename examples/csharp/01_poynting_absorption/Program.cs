using OpticsExamples.Common;
using OpticsToolkits;

const double Pi = Math.PI;

using var stack = new LayerStack();
stack.AddLayer(1.0, 0.0, 0.0);
stack.AddLayer(2.2, 0.2, 100.0);
stack.AddLayer(3.3, 0.3, 300.0);
stack.AddLayer(1.0, 0.0, 0.0);

const double th0 = Pi / 4.0;
const double lambda = 400.0;
var dir = Tmm.PropagateDirection(stack, th0);

var iface = Tmm.InterfaceTransferMatrixP(stack, dir, lambda);
var applied = iface.ToArray();
Tmm.ApplyMultilayerEffect(applied);
var tmmLast = applied[^1];

var (r, t) = Tmm.GetRTFromTmm(tmmLast);
var vwList = new OpticsComplex[iface.Length + 1][];
for (var i = 0; i <= iface.Length; i++)
    vwList[i] = new OpticsComplex[2];

vwList[iface.Length][0] = t;
vwList[iface.Length][1] = new OpticsComplex(0.0, 0.0);
for (var i = iface.Length - 1; i >= 0; i--)
    Tmm.MatVec(iface[i], vwList[i + 1][0], vwList[i + 1][1], out vwList[i][0], out vwList[i][1]);

var pos = Tmm.GetStructurePos(stack);

const int N = 240;
var ds = new double[N];
var poyn = new double[N];
var absor = new double[N];
var ex = new double[N];
var ey = new double[N];
var ez = new double[N];

for (var i = 0; i < N; i++)
{
    ds[i] = -50.0 + (550.0 * i) / (N - 1);
    var (layerId, depthIn) = Tmm.FindInStructure(pos, ds[i]);
    var phase = Tmm.PropagateMatrixToDepth(lambda, dir, stack, layerId, depthIn);
    var vf0 = vwList[layerId][0];
    var vb0 = vwList[layerId][1];
    Tmm.MatVec(phase, vf0, vb0, out var ef, out var eb);

    var layer = stack.GetLayer(layerId);
    poyn[i] = Tmm.GetPoyntingVectorP(ef, eb, dir[layerId], layer.Nk, dir[0], new OpticsComplex(1.0, 0.0));
    absor[i] = Tmm.GetAbsorbedEnergyDensityP(ef, eb, lambda, dir, stack, layerId);
    var efield = Tmm.GetEFieldP(ef, eb, dir[layerId]);
    ex[i] = efield.X.Re;
    ey[i] = efield.Y.Re;
    ez[i] = efield.Z.Re;
}

var absorScaled = absor.Select(a => 200.0 * a).ToArray();

OutputHelper.EnsureOutputDir();
OutputHelper.WriteCsvMulti("output/01_poynting_absorption.csv", "depth-z (nm)",
    ["Poynting", "200*absorption"], ds, [poyn, absorScaled]);
OutputHelper.WriteCsvMulti("output/01_e_field.csv", "depth-z (nm)",
    ["ex", "ey", "ez"], ds, [ex, ey, ez]);

return 0;
