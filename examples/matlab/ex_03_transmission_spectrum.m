% Ex3: Transmission vs wavelength with interpolated nk.
wlTab = [200, 300, 400, 500, 750];
nkTab = [2.1+0.1i, 2.4+0.3i, 2.3+0.4i, 2.2+0.4i, 2.2+0.5i];
depth = 300;
N = 400;
lambda = 200 + (550 * (0:N-1)) / (N - 1);
T = zeros(1, N);

for i = 1:N
    stack = optics.LayerStack();
    stack.addLayer(1.0, 0.0, 0.0);
    nk = interpNkQuadratic(wlTab, nkTab, lambda(i));
    stack.addLayer(real(nk), imag(nk), depth);
    stack.addLayer(1.0, 0.0, 0.0);
    h = stack.getHandle();
    dir = optics.tmm('propagate_direction', h, 0);
    tmm = optics.tmm('last_matrix_s', h, dir, lambda(i));
    [~, T(i)] = optics.tmm('get_r_t_power_s', tmm, 1, dir(1), 1, dir(end));
end

optics.write_csv('output/03_transmission_spectrum.csv', 'Wavelength (nm)', ...
    {'Fraction of power transmitted'}, lambda, {T});

function nk = interpNkQuadratic(wl, nk, x)
    if numel(wl) < 3 || x <= wl(1), nk = nk(1); return; end
    if x >= wl(end), nk = nk(end); return; end
    for i = 2:numel(wl)-1
        if x >= wl(i-1) && x <= wl(i+1)
            x0 = wl(i-1); x1 = wl(i); x2 = wl(i+1);
            l0 = (x-x1)*(x-x2)/((x0-x1)*(x0-x2));
            l1 = (x-x0)*(x-x2)/((x1-x0)*(x1-x2));
            l2 = (x-x0)*(x-x1)/((x2-x0)*(x2-x1));
            nk = l0*nk(i-1) + l1*nk(i) + l2*nk(i+1);
            return;
        end
    end
    nk = nk(round(end/2));
end
