% Ex2: Reflection spectrum at 0 and 45 degrees.
stack = optics.LayerStack();
stack.addLayer(1.0, 0.0, 0.0);
stack.addLayer(2.2, 0.0, 100.0);
stack.addLayer(3.3, 0.3, 300.0);
stack.addLayer(1.0, 0.0, 0.0);
h = stack.getHandle();

N = 400;
kMin = 0.0001; kMax = 0.01;
kcm = zeros(1, N);
Rnorm = zeros(1, N);
R45 = zeros(1, N);

for i = 1:N
    k = kMin + (kMax - kMin) * (i - 1) / (N - 1);
    wl = 1 / k;
    kcm(i) = k * 1e7;
    dir = optics.tmm('propagate_direction', h, 0);
    tmmS = optics.tmm('last_matrix_s', h, dir, wl);
    Rnorm(i) = optics.tmm('get_r_t_power_s', tmmS, 1, dir(1), 1, dir(end));
    dir = optics.tmm('propagate_direction', h, pi/4);
    tmmS = optics.tmm('last_matrix_s', h, dir, wl);
    tmmP = optics.tmm('last_matrix_p', h, dir, wl);
    R45(i) = optics.tmm('get_unpolarized_r_t_power', tmmS, tmmP, 1, dir(1), 1, dir(end));
end

optics.write_csv('output/02_reflection_spectrum.csv', 'k (cm^-1)', ...
    {'Rnorm', 'R45'}, kcm, {Rnorm, R45});
