using OpticsExamples.Common;
using OpticsToolkits;

using var stack = new LayerStack();
stack.AddLayer(1.517, 0.0, 0.0);
stack.AddLayer(3.719, 4.362, 5.0);
stack.AddLayer(0.130, 3.162, 30.0);
stack.AddLayer(1.0, 0.0, 0.0);

var l0 = stack.GetLayer(0);
var l3 = stack.GetLayer(3);

const double lambda = 633.0;
const int N = 300;
var thetaDeg = new double[N];
var rp = new double[N];
var tp = new double[N];

for (var i = 0; i < N; i++)
{
    var theta = (30.0 + (30.0 * i) / (N - 1)) * Math.PI / 180.0;
    thetaDeg[i] = theta * 180.0 / Math.PI;
    var dir = Tmm.PropagateDirection(stack, theta);
    var theta0 = dir[0];
    var theta3 = dir[3];
    Tmm.ApplySprEffect(l0.Nk, ref theta0);
    Tmm.ApplySprEffect(l3.Nk, ref theta3);
    dir[0] = theta0;
    dir[3] = theta3;

    var tmm = Tmm.LastMatrixP(stack, dir, lambda);
    var (rPower, tPower) = Tmm.GetRTPowerFromTmmP(tmm, l0.Nk, dir[0], l3.Nk, dir[3]);
    rp[i] = rPower;
    tp[i] = tPower;
}

OutputHelper.EnsureOutputDir();
OutputHelper.WriteCsvMulti("output/05_surface_plasmon_resonance.csv", "theta (degree)",
    ["Rp", "Tp"], thetaDeg, [rp, tp]);

return 0;
