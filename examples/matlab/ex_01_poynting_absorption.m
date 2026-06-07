% Ex1: Poynting vector and absorption vs depth (CSV output).
stack = optics.LayerStack();
stack.addLayer(1.0, 0.0, 0.0);
stack.addLayer(2.2, 0.2, 100.0);
stack.addLayer(3.3, 0.3, 300.0);
stack.addLayer(1.0, 0.0, 0.0);
h = stack.getHandle();

th0 = pi / 4;
lambda = 400;
dir = optics.tmm('propagate_direction', h, th0);
iface = optics.tmm('interface_transfer_matrix_p', h, dir, lambda);
applied = optics.tmm('apply_multilayer_effect', iface);
tmmLast = applied{end};
[~, t] = optics.tmm('get_r_t_from_tmm', tmmLast);

nIface = numel(iface);
vw = cell(nIface + 1, 2);
vw{nIface + 1, 1} = t;
vw{nIface + 1, 2} = 0;
for i = nIface:-1:1
    [vf, vb] = matVec(iface{i}, vw{i+1, 1}, vw{i+1, 2});
    vw{i, 1} = vf;
    vw{i, 2} = vb;
end

pos = optics.tmm('get_structure_pos', h);
N = 240;
ds = -50 + (550 * (0:N-1)) / (N - 1);
poyn = zeros(1, N);
absor = zeros(1, N);
ex = zeros(1, N); ey = zeros(1, N); ez = zeros(1, N);

for i = 1:N
    [layerId, depthIn] = optics.tmm('find_in_structure', pos, ds(i));
    phase = optics.tmm('propagate_matrix_to_depth', h, dir, lambda, layerId, depthIn);
    [ef, eb] = matVec(phase, vw{layerId+1, 1}, vw{layerId+1, 2});
    poyn(i) = optics.tmm('get_poynting_vector_p', ef, eb, dir(layerId+1), ...
        complex(2.2, 0.2), dir(1), 1);
    absor(i) = optics.tmm('get_absorbed_energy_density_p', ef, eb, lambda, dir, h, layerId);
end

optics.write_csv('output/01_poynting_absorption.csv', 'depth-z (nm)', ...
    {'Poynting', '200*absorption'}, ds, {poyn, 200*absor});

function [of, ob] = matVec(M, vf, vb)
    of = M(1,1)*vf + M(1,2)*vb;
    ob = M(2,1)*vf + M(2,2)*vb;
end
