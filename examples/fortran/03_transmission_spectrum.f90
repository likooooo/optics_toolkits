program ex03_transmission_spectrum
  use optics_toolkits
  use csv_output
  implicit none

  type(optics_layer_stack) :: stack
  type(optics_complex) :: dir(3), crao, nk_val
  type(optics_matrix2x2) :: tmm
  type(optics_layer) :: l0, l2
  real(8) :: wl_tab(5), lambda(400), tval(400), rp, tp
  type(optics_complex) :: nk_tab(5)
  real(8), parameter :: depth = 300.0d0
  integer :: n_dir, i

  wl_tab = (/200.0d0, 300.0d0, 400.0d0, 500.0d0, 750.0d0/)
  nk_tab(1) = oc(2.1d0, 0.1d0)
  nk_tab(2) = oc(2.4d0, 0.3d0)
  nk_tab(3) = oc(2.3d0, 0.4d0)
  nk_tab(4) = oc(2.2d0, 0.4d0)
  nk_tab(5) = oc(2.2d0, 0.5d0)

  call ensure_output_dir('output')

  do i = 1, 400
    lambda(i) = 200.0d0 + (550.0d0 * dble(i - 1)) / 399.0d0

    call stack%create()
    call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))
    nk_val = interp_nk_quadratic(wl_tab, nk_tab, 5, lambda(i))
    call stack%add_layer(ol(nk_val%re, nk_val%im, depth))
    call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))

    crao = oc(0.0d0, 0.0d0)
    n_dir = 3
    call tmm_propagate_direction(stack, crao, dir, n_dir)
    call tmm_last_matrix_s(stack, dir, n_dir, lambda(i), tmm)
    l0 = stack%get_layer(1)
    l2 = stack%get_layer(3)
    call tmm_get_r_t_power_from_tmm_s(tmm, l0%nk, dir(1), l2%nk, dir(3), rp, tp)
    tval(i) = tp
  end do

  call write_csv_2col('output/03_transmission_spectrum.csv', 'Wavelength (nm)', &
    'Fraction of power transmitted', lambda, tval, 400)
end program ex03_transmission_spectrum
