program ex02_reflection_spectrum
  use optics_toolkits
  use csv_output
  implicit none

  type(optics_layer_stack) :: stack
  type(optics_complex) :: dir(4), crao
  type(optics_matrix2x2) :: tmm_s, tmm_p
  type(optics_layer) :: layer0, layer_last
  real(8) :: kcm(400), rnorm(400), r45(400), k, wl, rp, tp
  real(8), parameter :: k_min = 0.0001d0, k_max = 0.01d0
  integer :: n_dir, i
  character(len=16) :: ynames(2)

  call ensure_output_dir('output')

  call stack%create()
  call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))
  call stack%add_layer(ol(2.2d0, 0.0d0, 100.0d0))
  call stack%add_layer(ol(3.3d0, 0.3d0, 300.0d0))
  call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))

  layer0 = stack%get_layer(1)
  layer_last = stack%get_layer(4)

  do i = 1, 400
    k = k_min + (k_max - k_min) * dble(i - 1) / 399.0d0
    wl = 1.0d0 / k
    kcm(i) = k * 1.0d7

    crao = oc(0.0d0, 0.0d0)
    n_dir = 4
    call tmm_propagate_direction(stack, crao, dir, n_dir)
    call tmm_last_matrix_s(stack, dir, n_dir, wl, tmm_s)
    call tmm_get_r_t_power_from_tmm_s(tmm_s, layer0%nk, dir(1), layer_last%nk, dir(4), rp, tp)
    rnorm(i) = rp

    crao = oc(PI / 4.0d0, 0.0d0)
    call tmm_propagate_direction(stack, crao, dir, n_dir)
    call tmm_last_matrix_s(stack, dir, n_dir, wl, tmm_s)
    call tmm_last_matrix_p(stack, dir, n_dir, wl, tmm_p)
    call tmm_get_unpolarized_r_t_power(tmm_s, tmm_p, layer0%nk, dir(1), layer_last%nk, dir(4), rp, tp)
    r45(i) = rp
  end do

  ynames(1) = 'Rnorm'
  ynames(2) = 'R45'
  block
    real(8) :: y2d(2, 400)
    y2d(1, :) = rnorm
    y2d(2, :) = r45
    call write_csv_multi('output/02_reflection_spectrum.csv', 'k (cm^-1)', ynames, 2, kcm, y2d, 400)
  end block
end program ex02_reflection_spectrum
