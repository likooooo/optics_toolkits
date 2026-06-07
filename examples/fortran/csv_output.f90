! Shared CSV writers for Fortran examples (no Python/plugins dependency).
module csv_output
  implicit none
  private
  public :: ensure_output_dir, write_csv_2col, write_csv_multi

contains

  subroutine ensure_output_dir(path)
    character(len=*), intent(in) :: path
    character(len=256) :: cmd
    write(cmd, '(A)') 'mkdir -p '//trim(path)
    call execute_command_line(trim(cmd))
  end subroutine ensure_output_dir

  subroutine write_csv_2col(path, x_name, y_name, x, y, n)
    character(len=*), intent(in) :: path, x_name, y_name
    integer, intent(in) :: n
    real(8), intent(in) :: x(n), y(n)
    integer :: u, i
    open(newunit=u, file=path, status='replace', action='write')
    write(u, '(A,A,A)') trim(x_name), ',', trim(y_name)
    do i = 1, n
      write(u, '(ES23.15,ES23.15)') x(i), y(i)
    end do
    close(u)
    write(*, '(A,A)') 'wrote ', trim(path)
  end subroutine write_csv_2col

  subroutine write_csv_multi(path, x_name, y_names, n_curves, x, y, n)
    character(len=*), intent(in) :: path, x_name
    character(len=*), intent(in) :: y_names(*)
    integer, intent(in) :: n_curves, n
    real(8), intent(in) :: x(n), y(n_curves, n)
    integer :: u, i, c
    open(newunit=u, file=path, status='replace', action='write')
    write(u, '(A)', advance='no') trim(x_name)
    do c = 1, n_curves
      write(u, '(A,A)', advance='no') ',', trim(y_names(c))
    end do
    write(u, *)
    do i = 1, n
      write(u, '(ES23.15)', advance='no') x(i)
      do c = 1, n_curves
        write(u, '(A,ES23.15)', advance='no') ',', y(c, i)
      end do
      write(u, *)
    end do
    close(u)
    write(*, '(A,A)') 'wrote ', trim(path)
  end subroutine write_csv_multi

end module csv_output
