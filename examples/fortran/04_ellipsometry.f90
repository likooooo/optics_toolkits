program ex04_ellipsometry
  use optics_toolkits
  use csv_output
  implicit none

  type(optics_layer_stack) :: stack
  real(8) :: ds(100), psis(100), deltas(100), psi, delta
  real(8), parameter :: crao = 70.0d0 * PI / 180.0d0
  real(8), parameter :: wavelength = 633.0d0
  integer :: i
  character(len=16) :: ynames(2)

  call ensure_output_dir('output')
  call stack%create()

  do i = 1, 100
    ds(i) = (1000.0d0 * dble(i - 1)) / 99.0d0
    call stack%destroy()
    call stack%create()
    call stack%add_layer(ol(1.0d0, 0.0d0, 0.0d0))
    call stack%add_layer(ol(1.46d0, 0.0d0, ds(i)))
    call stack%add_layer(ol(3.87d0, 0.02d0, 0.0d0))
    call tmm_ellipsometric_psi_delta(stack, crao, wavelength, psi, delta)
    psis(i) = psi * 180.0d0 / PI
    deltas(i) = delta * 180.0d0 / PI
  end do

  ynames(1) = 'Psi'
  ynames(2) = 'Delta'
  block
    real(8) :: y2d(2, 100)
    y2d(1, :) = psis
    y2d(2, :) = deltas
    call write_csv_multi('output/04_ellipsometry.csv', 'SiO2 thickness (nm)', ynames, 2, ds, y2d, 100)
  end block
end program ex04_ellipsometry
