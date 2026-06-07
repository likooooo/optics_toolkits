program ex01_poynting_absorption
  use optics_toolkits
  use csv_output
  implicit none

  type(optics_layer_stack) :: stack
  type(optics_complex) :: dir(4), crao, r, t, vf0, vb0, ef, eb
  type(optics_complex) :: vw_list(9, 2)
  type(optics_matrix2x2) :: iface(8), applied(8), tmm_last, phase
  type(optics_layer) :: layer
  type(optics_vec3) :: efield
  real(8) :: pos(8), ds(240), poyn(240), absor(240), ex(240), ey(240), ez(240)
  real(8) :: absor_scaled(240), th0, lambda, depth_in
  integer :: n_dir, n_iface, n_pos, layer_id, i
  character(len=32) :: ynames(3)

  call ensure_output_dir('output')

  call stack%create()
  call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))
  call stack%add_layer(ol(2.2d0, 0.2d0, 100.0d0))
  call stack%add_layer(ol(3.3d0, 0.3d0, 300.0d0))
  call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))

  th0 = PI / 4.0d0
  lambda = 400.0d0
  crao = oc(th0, 0.0d0)
  n_dir = 4
  call tmm_propagate_direction(stack, crao, dir, n_dir)

  n_iface = 8
  call tmm_interface_transfer_matrix_p(stack, dir, n_dir, lambda, iface, n_iface)
  applied(1:n_iface) = iface(1:n_iface)
  call tmm_apply_multilayer_effect(applied, n_iface)
  tmm_last = applied(n_iface)
  call tmm_get_r_t_from_tmm(tmm_last, r, t)

  vw_list(n_iface + 1, 1) = t
  vw_list(n_iface + 1, 2) = oc(0.0d0, 0.0d0)
  do i = n_iface, 1, -1
    call mat_vec(iface(i), vw_list(i+1, 1), vw_list(i+1, 2), vw_list(i, 1), vw_list(i, 2))
  end do

  n_pos = 8
  call tmm_get_structure_pos(stack, pos, n_pos)

  do i = 1, 240
    ds(i) = -50.0d0 + (550.0d0 * dble(i - 1)) / 239.0d0
    call tmm_find_in_structure(pos, n_pos, ds(i), layer_id, depth_in)
    call tmm_propagate_matrix_to_depth(lambda, dir, n_dir, stack, layer_id, depth_in, phase)
    vf0 = vw_list(layer_id, 1)
    vb0 = vw_list(layer_id, 2)
    call mat_vec(phase, vf0, vb0, ef, eb)
    layer = stack%get_layer(layer_id)
    call tmm_get_poynting_vector_p(ef, eb, dir(layer_id), layer%nk, dir(1), oc(1.0d0, 0.0d0), poyn(i))
    call tmm_get_absorbed_energy_density_p(ef, eb, lambda, dir, n_dir, stack, layer_id, absor(i))
    call tmm_get_e_field_p(ef, eb, dir(layer_id), efield)
    ex(i) = efield%x%re
    ey(i) = efield%y%re
    ez(i) = efield%z%re
  end do

  do i = 1, 240
    absor_scaled(i) = 200.0d0 * absor(i)
  end do

  ynames(1) = 'Poynting'
  ynames(2) = '200*absorption'
  block
    real(8) :: y2d(2, 240)
    y2d(1, :) = poyn
    y2d(2, :) = absor_scaled
    call write_csv_multi('output/01_poynting_absorption.csv', 'depth-z (nm)', ynames, 2, ds, y2d, 240)
  end block

  ynames(1) = 'ex'
  ynames(2) = 'ey'
  ynames(3) = 'ez'
  block
    real(8) :: y3d(3, 240)
    y3d(1, :) = ex
    y3d(2, :) = ey
    y3d(3, :) = ez
    call write_csv_multi('output/01_e_field.csv', 'depth-z (nm)', ynames, 3, ds, y3d, 240)
  end block
end program ex01_poynting_absorption
