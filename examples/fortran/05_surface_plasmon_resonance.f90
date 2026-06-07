program ex05_surface_plasmon_resonance
  use optics_toolkits
  use csv_output
  implicit none

  type(optics_layer_stack) :: stack
  type(optics_complex) :: dir(4), theta_in
  type(optics_matrix2x2) :: tmm
  type(optics_layer) :: l0, l3
  real(8) :: theta_deg(300), rp(300), tp(300), theta, rpow, tpow
  real(8), parameter :: lambda = 633.0d0
  integer :: n_dir, i
  character(len=8) :: ynames(2)

  call ensure_output_dir('output')

  call stack%create()
  call stack%add_layer(ol(1.517d0, 0.0d0, 0.0d0))
  call stack%add_layer(ol(3.719d0, 4.362d0, 5.0d0))
  call stack%add_layer(ol(0.130d0, 3.162d0, 30.0d0))
  call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))

  l0 = stack%get_layer(1)
  l3 = stack%get_layer(4)

  do i = 1, 300
    theta = (30.0d0 + (30.0d0 * dble(i - 1)) / 299.0d0) * PI / 180.0d0
    theta_deg(i) = theta * 180.0d0 / PI
    theta_in = oc(theta, 0.0d0)
    n_dir = 4
    call tmm_propagate_direction(stack, theta_in, dir, n_dir)
    call tmm_apply_spr_effect(l0%nk, dir(1), dir(1))
    call tmm_apply_spr_effect(l3%nk, dir(4), dir(4))
    call tmm_last_matrix_p(stack, dir, n_dir, lambda, tmm)
    call tmm_get_r_t_power_from_tmm_p(tmm, l0%nk, dir(1), l3%nk, dir(4), rpow, tpow)
    rp(i) = rpow
    tp(i) = tpow
  end do

  ynames(1) = 'Rp'
  ynames(2) = 'Tp'
  block
    real(8) :: y2d(2, 300)
    y2d(1, :) = rp
    y2d(2, :) = tp
    call write_csv_multi('output/05_surface_plasmon_resonance.csv', 'theta (degree)', ynames, 2, theta_deg, y2d, 300)
  end block
end program ex05_surface_plasmon_resonance
