% Ex5: SPR reflection/transmission vs angle.
stack = optics.LayerStack();
stack.addLayer(1.517, 0.0, 0.0);
stack.addLayer(3.719, 4.362, 5.0);
stack.addLayer(0.130, 3.162, 30.0);
stack.addLayer(1.0, 0.0, 0.0);
h = stack.getHandle();
lambda = 633;
N = 300;
thetaDeg = zeros(1, N);
Rp = zeros(1, N);
Tp = zeros(1, N);

for i = 1:N
    theta = (30 + 30 * (i - 1) / (N - 1)) * pi / 180;
    thetaDeg(i) = theta * 180 / pi;
    dir = optics.tmm('propagate_direction', h, theta);
    dir(1) = optics.tmm('apply_spr_effect', 1.517, dir(1));
    dir(end) = optics.tmm('apply_spr_effect', 1.0, dir(end));
    tmm = optics.tmm('last_matrix_p', h, dir, lambda);
    [Rp(i), Tp(i)] = optics.tmm('get_r_t_power_p', tmm, 1.517, dir(1), 1.0, dir(end));
end

optics.write_csv('output/05_surface_plasmon_resonance.csv', 'theta (degree)', ...
    {'Rp', 'Tp'}, thetaDeg, {Rp, Tp});
