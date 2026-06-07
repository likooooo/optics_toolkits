% Ex4: Ellipsometric Psi/Delta vs SiO2 thickness.
crao = 70 * pi / 180;
wavelength = 633;
N = 100;
ds = (1000 * (0:N-1)) / (N - 1);
psis = zeros(1, N);
deltas = zeros(1, N);

for i = 1:N
    stack = optics.LayerStack();
    stack.addLayer(1.0, 0.0, 0.0);
    stack.addLayer(1.46, 0.0, ds(i));
    stack.addLayer(3.87, 0.02, 0.0);
    h = stack.getHandle();
    [psi, delta] = optics.tmm('ellipsometric_psi_delta', h, crao, wavelength);
    psis(i) = psi * 180 / pi;
    deltas(i) = delta * 180 / pi;
end

optics.write_csv('output/04_ellipsometry.csv', 'SiO2 thickness (nm)', ...
    {'Psi', 'Delta'}, ds, {psis, deltas});
