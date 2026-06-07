function varargout = tmm(cmd, varargin)
%TMM Thin-film transfer-matrix helpers via optics_tmm_mex.
%   H = optics.tmm('create')
%   optics.tmm('destroy', H)
%   optics.tmm('add_layer', H, nk_re, nk_im, depth)
%   DIR = optics.tmm('propagate_direction', H, crao_rad)
%   M = optics.tmm('last_matrix_s', H, dir, wavelength)
%   [PSI, DELTA] = optics.tmm('ellipsometric_psi_delta', H, crao, wl)
    if nargin < 1
        error('optics:tmm:arg', 'command string required');
    end
    [varargout{1:nargout}] = optics_tmm_mex(cmd, varargin{:});
end
