! Fortran iso_c_binding wrappers for optics_toolkits TMM C ABI.
! Compatible with gfortran and Intel ifx.

module optics_toolkits
  use, intrinsic :: iso_c_binding
  implicit none

  private

  ! --- C-interoperable types (optics_types.h) ---

  type, bind(c), public :: optics_complex
    real(c_double) :: re
    real(c_double) :: im
  end type optics_complex

  type, bind(c), public :: optics_layer
    type(optics_complex) :: nk
    real(c_double) :: depth
  end type optics_layer

  ! Row-major 2x2 layout: m(1)=m[0][0], m(2)=m[0][1], m(3)=m[1][0], m(4)=m[1][1]
  type, bind(c), public :: optics_matrix2x2
    type(optics_complex) :: m(4)
  end type optics_matrix2x2

  type, bind(c), public :: optics_vec3
    type(optics_complex) :: x
    type(optics_complex) :: y
    type(optics_complex) :: z
  end type optics_vec3

  integer, parameter, public :: OPTICS_OK              = 0
  integer, parameter, public :: OPTICS_ERR_INVALID_ARG = 1
  integer, parameter, public :: OPTICS_ERR_OUT_OF_MEMORY = 2
  integer, parameter, public :: OPTICS_ERR_INTERNAL    = 3
  integer, parameter, public :: OPTICS_ERR_NOT_AVAILABLE = 4

  integer, parameter, public :: OPTICS_PRECISION_F32 = 0
  integer, parameter, public :: OPTICS_PRECISION_F64 = 1

  ! --- Opaque layer stack with automatic cleanup ---

  type, public :: optics_layer_stack
    type(c_ptr) :: handle = c_null_ptr
  contains
    procedure :: create => layer_stack_create
    procedure :: add_layer => layer_stack_add_layer
    procedure :: layer_count => layer_stack_layer_count
    procedure :: get_layer => layer_stack_get_layer
    procedure :: destroy => layer_stack_destroy
    final :: layer_stack_final
  end type optics_layer_stack

  ! --- C API (optics_toolkits.h) ---

  interface
    function c_optics_get_version() bind(c, name='optics_get_version')
      import :: c_ptr
      type(c_ptr) :: c_optics_get_version
    end function c_optics_get_version

    function c_optics_get_api_version() bind(c, name='optics_get_api_version')
      import :: c_int
      integer(c_int) :: c_optics_get_api_version
    end function c_optics_get_api_version

    function c_optics_get_precision() bind(c, name='optics_get_precision')
      import :: c_int
      integer(c_int) :: c_optics_get_precision
    end function c_optics_get_precision

    function c_optics_last_error() bind(c, name='optics_last_error')
      import :: c_ptr
      type(c_ptr) :: c_optics_last_error
    end function c_optics_last_error

    subroutine c_optics_clear_error() bind(c, name='optics_clear_error')
    end subroutine c_optics_clear_error
  end interface

  ! --- C API (optics_tmm.h) ---

  interface
    function c_optics_layer_stack_create() bind(c, name='optics_layer_stack_create')
      import :: c_ptr
      type(c_ptr) :: c_optics_layer_stack_create
    end function c_optics_layer_stack_create

    function c_optics_layer_stack_add_layer(stack, layer) &
        bind(c, name='optics_layer_stack_add_layer')
      import :: c_ptr, c_int, optics_layer
      type(c_ptr), value :: stack
      type(optics_layer), value :: layer
      integer(c_int) :: c_optics_layer_stack_add_layer
    end function c_optics_layer_stack_add_layer

    function c_optics_layer_stack_layer_count(stack) &
        bind(c, name='optics_layer_stack_layer_count')
      import :: c_ptr, c_size_t
      type(c_ptr), value :: stack
      integer(c_size_t) :: c_optics_layer_stack_layer_count
    end function c_optics_layer_stack_layer_count

    function c_optics_layer_stack_get_layer(stack, index, out) &
        bind(c, name='optics_layer_stack_get_layer')
      import :: c_ptr, c_int, c_size_t, optics_layer
      type(c_ptr), value :: stack
      integer(c_size_t), value :: index
      type(optics_layer) :: out
      integer(c_int) :: c_optics_layer_stack_get_layer
    end function c_optics_layer_stack_get_layer

    subroutine c_optics_layer_stack_destroy(stack) bind(c, name='optics_layer_stack_destroy')
      import :: c_ptr
      type(c_ptr), value :: stack
    end subroutine c_optics_layer_stack_destroy

    function c_optics_tmm_propagate_direction(stack, crao, out_angles, out_count) &
        bind(c, name='optics_tmm_propagate_direction')
      import :: c_ptr, c_int, c_size_t, optics_complex
      type(c_ptr), value :: stack
      type(optics_complex), value :: crao
      type(optics_complex) :: out_angles(*)
      integer(c_size_t) :: out_count
      integer(c_int) :: c_optics_tmm_propagate_direction
    end function c_optics_tmm_propagate_direction

    function c_optics_tmm_interface_transfer_matrix_s(stack, propagate_dir, dir_count, &
        wavelength, out_matrices, out_count) &
        bind(c, name='optics_tmm_interface_transfer_matrix_s')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex, optics_matrix2x2
      type(c_ptr), value :: stack
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      real(c_double), value :: wavelength
      type(optics_matrix2x2) :: out_matrices(*)
      integer(c_size_t) :: out_count
      integer(c_int) :: c_optics_tmm_interface_transfer_matrix_s
    end function c_optics_tmm_interface_transfer_matrix_s

    function c_optics_tmm_interface_transfer_matrix_p(stack, propagate_dir, dir_count, &
        wavelength, out_matrices, out_count) &
        bind(c, name='optics_tmm_interface_transfer_matrix_p')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex, optics_matrix2x2
      type(c_ptr), value :: stack
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      real(c_double), value :: wavelength
      type(optics_matrix2x2) :: out_matrices(*)
      integer(c_size_t) :: out_count
      integer(c_int) :: c_optics_tmm_interface_transfer_matrix_p
    end function c_optics_tmm_interface_transfer_matrix_p

    function c_optics_tmm_interface_transfer_matrix_with_thickness_s(stack, propagate_dir, &
        dir_count, wavelength, out_matrices, out_count) &
        bind(c, name='optics_tmm_interface_transfer_matrix_with_thickness_s')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex, optics_matrix2x2
      type(c_ptr), value :: stack
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      real(c_double), value :: wavelength
      type(optics_matrix2x2) :: out_matrices(*)
      integer(c_size_t) :: out_count
      integer(c_int) :: c_optics_tmm_interface_transfer_matrix_with_thickness_s
    end function c_optics_tmm_interface_transfer_matrix_with_thickness_s

    function c_optics_tmm_interface_transfer_matrix_with_thickness_p(stack, propagate_dir, &
        dir_count, wavelength, out_matrices, out_count) &
        bind(c, name='optics_tmm_interface_transfer_matrix_with_thickness_p')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex, optics_matrix2x2
      type(c_ptr), value :: stack
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      real(c_double), value :: wavelength
      type(optics_matrix2x2) :: out_matrices(*)
      integer(c_size_t) :: out_count
      integer(c_int) :: c_optics_tmm_interface_transfer_matrix_with_thickness_p
    end function c_optics_tmm_interface_transfer_matrix_with_thickness_p

    function c_optics_tmm_apply_multilayer_effect(matrices, count) &
        bind(c, name='optics_tmm_apply_multilayer_effect')
      import :: c_int, c_size_t, optics_matrix2x2
      type(optics_matrix2x2) :: matrices(*)
      integer(c_size_t), value :: count
      integer(c_int) :: c_optics_tmm_apply_multilayer_effect
    end function c_optics_tmm_apply_multilayer_effect

    function c_optics_tmm_get_structure_pos(stack, out_pos, out_count) &
        bind(c, name='optics_tmm_get_structure_pos')
      import :: c_ptr, c_int, c_size_t, c_double
      type(c_ptr), value :: stack
      real(c_double) :: out_pos(*)
      integer(c_size_t) :: out_count
      integer(c_int) :: c_optics_tmm_get_structure_pos
    end function c_optics_tmm_get_structure_pos

    function c_optics_tmm_find_in_structure(structure_pos, pos_count, depth, out_index, &
        out_depth_in_structure) bind(c, name='optics_tmm_find_in_structure')
      import :: c_int, c_size_t, c_double
      real(c_double) :: structure_pos(*)
      integer(c_size_t), value :: pos_count
      real(c_double), value :: depth
      integer(c_size_t) :: out_index
      real(c_double) :: out_depth_in_structure
      integer(c_int) :: c_optics_tmm_find_in_structure
    end function c_optics_tmm_find_in_structure

    function c_optics_tmm_propagate_matrix_to_depth(wavelength, propagate_dir, dir_count, &
        stack, index, depth_in_structure, out) &
        bind(c, name='optics_tmm_propagate_matrix_to_depth')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex, optics_matrix2x2
      real(c_double), value :: wavelength
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      type(c_ptr), value :: stack
      integer(c_size_t), value :: index
      real(c_double), value :: depth_in_structure
      type(optics_matrix2x2) :: out
      integer(c_int) :: c_optics_tmm_propagate_matrix_to_depth
    end function c_optics_tmm_propagate_matrix_to_depth

    function c_optics_tmm_get_r_t_from_tmm(tmm, out_r, out_t) &
        bind(c, name='optics_tmm_get_r_t_from_tmm')
      import :: c_int, optics_complex, optics_matrix2x2
      type(optics_matrix2x2), value :: tmm
      type(optics_complex) :: out_r
      type(optics_complex) :: out_t
      integer(c_int) :: c_optics_tmm_get_r_t_from_tmm
    end function c_optics_tmm_get_r_t_from_tmm

    function c_optics_tmm_get_r_t_power_from_tmm_s(tmm, n1, theta1, n2, theta2, &
        out_r_power, out_t_power) bind(c, name='optics_tmm_get_r_t_power_from_tmm_s')
      import :: c_int, c_double, optics_complex, optics_matrix2x2
      type(optics_matrix2x2), value :: tmm
      type(optics_complex), value :: n1, theta1, n2, theta2
      real(c_double) :: out_r_power
      real(c_double) :: out_t_power
      integer(c_int) :: c_optics_tmm_get_r_t_power_from_tmm_s
    end function c_optics_tmm_get_r_t_power_from_tmm_s

    function c_optics_tmm_get_r_t_power_from_tmm_p(tmm, n1, theta1, n2, theta2, &
        out_r_power, out_t_power) bind(c, name='optics_tmm_get_r_t_power_from_tmm_p')
      import :: c_int, c_double, optics_complex, optics_matrix2x2
      type(optics_matrix2x2), value :: tmm
      type(optics_complex), value :: n1, theta1, n2, theta2
      real(c_double) :: out_r_power
      real(c_double) :: out_t_power
      integer(c_int) :: c_optics_tmm_get_r_t_power_from_tmm_p
    end function c_optics_tmm_get_r_t_power_from_tmm_p

    function c_optics_tmm_get_unpolarized_r_t_power(tmm_s, tmm_p, n1, theta1, n2, theta2, &
        out_r_power, out_t_power) bind(c, name='optics_tmm_get_unpolarized_r_t_power')
      import :: c_int, c_double, optics_complex, optics_matrix2x2
      type(optics_matrix2x2), value :: tmm_s, tmm_p
      type(optics_complex), value :: n1, theta1, n2, theta2
      real(c_double) :: out_r_power
      real(c_double) :: out_t_power
      integer(c_int) :: c_optics_tmm_get_unpolarized_r_t_power
    end function c_optics_tmm_get_unpolarized_r_t_power

    function c_optics_tmm_apply_spr_effect(nk, theta, out_theta) &
        bind(c, name='optics_tmm_apply_spr_effect')
      import :: c_int, optics_complex
      type(optics_complex), value :: nk
      type(optics_complex), value :: theta
      type(optics_complex) :: out_theta
      integer(c_int) :: c_optics_tmm_apply_spr_effect
    end function c_optics_tmm_apply_spr_effect

    function c_optics_tmm_get_e_field_s(e_forward, e_backward, theta, out) &
        bind(c, name='optics_tmm_get_e_field_s')
      import :: c_int, optics_complex, optics_vec3
      type(optics_complex), value :: e_forward, e_backward, theta
      type(optics_vec3) :: out
      integer(c_int) :: c_optics_tmm_get_e_field_s
    end function c_optics_tmm_get_e_field_s

    function c_optics_tmm_get_e_field_p(e_forward, e_backward, theta, out) &
        bind(c, name='optics_tmm_get_e_field_p')
      import :: c_int, optics_complex, optics_vec3
      type(optics_complex), value :: e_forward, e_backward, theta
      type(optics_vec3) :: out
      integer(c_int) :: c_optics_tmm_get_e_field_p
    end function c_optics_tmm_get_e_field_p

    function c_optics_tmm_get_poynting_vector_s(e_forward, e_backward, theta, n, theta0, n0, &
        out) bind(c, name='optics_tmm_get_poynting_vector_s')
      import :: c_int, c_double, optics_complex
      type(optics_complex), value :: e_forward, e_backward, theta, n, theta0, n0
      real(c_double) :: out
      integer(c_int) :: c_optics_tmm_get_poynting_vector_s
    end function c_optics_tmm_get_poynting_vector_s

    function c_optics_tmm_get_poynting_vector_p(e_forward, e_backward, theta, n, theta0, n0, &
        out) bind(c, name='optics_tmm_get_poynting_vector_p')
      import :: c_int, c_double, optics_complex
      type(optics_complex), value :: e_forward, e_backward, theta, n, theta0, n0
      real(c_double) :: out
      integer(c_int) :: c_optics_tmm_get_poynting_vector_p
    end function c_optics_tmm_get_poynting_vector_p

    function c_optics_tmm_get_absorbed_energy_density_s(e_forward, e_backward, wavelength, &
        propagate_dir, dir_count, stack, index, out) &
        bind(c, name='optics_tmm_get_absorbed_energy_density_s')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex
      type(optics_complex), value :: e_forward, e_backward
      real(c_double), value :: wavelength
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      type(c_ptr), value :: stack
      integer(c_size_t), value :: index
      real(c_double) :: out
      integer(c_int) :: c_optics_tmm_get_absorbed_energy_density_s
    end function c_optics_tmm_get_absorbed_energy_density_s

    function c_optics_tmm_get_absorbed_energy_density_p(e_forward, e_backward, wavelength, &
        propagate_dir, dir_count, stack, index, out) &
        bind(c, name='optics_tmm_get_absorbed_energy_density_p')
      import :: c_ptr, c_int, c_size_t, c_double, optics_complex
      type(optics_complex), value :: e_forward, e_backward
      real(c_double), value :: wavelength
      type(optics_complex) :: propagate_dir(*)
      integer(c_size_t), value :: dir_count
      type(c_ptr), value :: stack
      integer(c_size_t), value :: index
      real(c_double) :: out
      integer(c_int) :: c_optics_tmm_get_absorbed_energy_density_p
    end function c_optics_tmm_get_absorbed_energy_density_p

    function c_optics_tmm_ellipsometric_psi_delta(stack, crao_rad, wavelength, out_psi, &
        out_delta) bind(c, name='optics_tmm_ellipsometric_psi_delta')
      import :: c_ptr, c_int, c_double
      type(c_ptr), value :: stack
      real(c_double), value :: crao_rad, wavelength
      real(c_double) :: out_psi
      real(c_double) :: out_delta
      integer(c_int) :: c_optics_tmm_ellipsometric_psi_delta
    end function c_optics_tmm_ellipsometric_psi_delta

    function c_optics_tmm_compute_forward_backward_at_depth(stack, crao_rad, wavelength, &
        polarization, depth, out_forward, out_backward) &
        bind(c, name='optics_tmm_compute_forward_backward_at_depth')
      import :: c_ptr, c_int, c_char, c_double, optics_complex
      type(c_ptr), value :: stack
      real(c_double), value :: crao_rad, wavelength, depth
      character(kind=c_char), value :: polarization
      type(optics_complex) :: out_forward
      type(optics_complex) :: out_backward
      integer(c_int) :: c_optics_tmm_compute_forward_backward_at_depth
    end function c_optics_tmm_compute_forward_backward_at_depth
  end interface

  public :: optics_get_version, optics_last_error_message, optics_check
  public :: ol, oc, matrix_get, matrix_set, mat_vec
  public :: tmm_propagate_direction, tmm_last_matrix_s, tmm_last_matrix_p
  public :: tmm_interface_transfer_matrix_p, tmm_apply_multilayer_effect
  public :: tmm_get_structure_pos, tmm_find_in_structure, tmm_propagate_matrix_to_depth
  public :: tmm_get_r_t_from_tmm, tmm_get_r_t_power_from_tmm_s, tmm_get_r_t_power_from_tmm_p
  public :: tmm_get_unpolarized_r_t_power, tmm_apply_spr_effect
  public :: tmm_get_e_field_p, tmm_get_poynting_vector_p, tmm_get_absorbed_energy_density_p
  public :: tmm_ellipsometric_psi_delta, tmm_get_e_field_s, tmm_get_poynting_vector_s
  public :: tmm_get_absorbed_energy_density_s
  public :: tmm_interface_transfer_matrix_s, tmm_interface_transfer_matrix_with_thickness_s
  public :: tmm_interface_transfer_matrix_with_thickness_p, interp_nk_quadratic
  public :: PI

  real(c_double), parameter :: PI = 3.14159265358979323846_c_double

contains

  ! --- Matrix helpers (row-major C layout) ---

  pure function matrix_get(mat, row, col) result(c)
    integer, intent(in) :: row, col
    type(optics_matrix2x2), intent(in) :: mat
    type(optics_complex) :: c
    c = mat%m(row*2 + col + 1)
  end function matrix_get

  pure subroutine matrix_set(mat, row, col, val)
    integer, intent(in) :: row, col
    type(optics_matrix2x2), intent(inout) :: mat
    type(optics_complex), intent(in) :: val
    mat%m(row*2 + col + 1) = val
  end subroutine matrix_set

  pure function ol(re, im, depth) result(layer)
    real(c_double), intent(in) :: re, im, depth
    type(optics_layer) :: layer
    layer%nk%re = re
    layer%nk%im = im
    layer%depth = depth
  end function ol

  pure function oc(re, im) result(c)
    real(c_double), intent(in) :: re, im
    type(optics_complex) :: c
    c%re = re
    c%im = im
  end function oc

  subroutine mat_vec(m, vf, vb, of, ob)
    type(optics_matrix2x2), intent(in) :: m
    type(optics_complex), intent(in) :: vf, vb
    type(optics_complex), intent(out) :: of, ob
    type(optics_complex) :: a, b, c, d, e, f
    a = matrix_get(m, 0, 0)
    b = matrix_get(m, 0, 1)
    c = matrix_get(m, 1, 0)
    d = matrix_get(m, 1, 1)
    of%re = a%re*vf%re - a%im*vf%im + b%re*vb%re - b%im*vb%im
    of%im = a%re*vf%im + a%im*vf%re + b%re*vb%im + b%im*vb%re
    ob%re = c%re*vf%re - c%im*vf%im + d%re*vb%re - d%im*vb%im
    ob%im = c%re*vf%im + c%im*vf%re + d%re*vb%im + d%im*vb%re
  end subroutine mat_vec

  function optics_get_version() result(ver)
    character(len=:), allocatable :: ver
    type(c_ptr) :: p
    character(kind=c_char, len=256) :: buf
    integer :: i
    p = c_optics_get_version()
    if (.not. c_associated(p)) then
      ver = ''
      return
    end if
    buf = ''
    call c_f_string(p, buf)
    i = index(buf, c_null_char)
    if (i > 0) then
      ver = buf(1:i-1)
    else
      ver = trim(buf)
    end if
  end function optics_get_version

  function optics_last_error_message() result(msg)
    character(len=:), allocatable :: msg
    type(c_ptr) :: p
    character(kind=c_char, len=1024) :: buf
    integer :: i
    p = c_optics_last_error()
    if (.not. c_associated(p)) then
      msg = ''
      return
    end if
    buf = ''
    call c_f_string(p, buf)
    i = index(buf, c_null_char)
    if (i > 0) then
      msg = buf(1:i-1)
    else
      msg = trim(buf)
    end if
  end function optics_last_error_message

  subroutine optics_check(status, context)
    integer, intent(in) :: status
    character(len=*), intent(in) :: context
    if (status /= OPTICS_OK) then
      write(*, '(A,I0,A)') 'optics_toolkits error in ', trim(context), ':'
      write(*, '(A)') trim(optics_last_error_message())
      stop 1
    end if
  end subroutine optics_check

  subroutine c_f_string(cstr, fstr)
    type(c_ptr), intent(in) :: cstr
    character(kind=c_char, len=*), intent(out) :: fstr
    character(kind=c_char), pointer :: p(:)
    integer :: n, i
    call c_f_pointer(cstr, p, [len(fstr, kind=c_size_t)])
    n = 0
    do i = 1, size(p)
      if (p(i) == c_null_char) exit
      n = n + 1
      fstr(n:n) = p(i)
    end do
    if (n < len(fstr)) fstr(n+1:) = ' '
  end subroutine c_f_string

  ! --- Layer stack methods ---

  subroutine layer_stack_create(self)
    class(optics_layer_stack), intent(inout) :: self
    if (c_associated(self%handle)) call layer_stack_destroy(self)
    self%handle = c_optics_layer_stack_create()
    if (.not. c_associated(self%handle)) then
      write(*, '(A)') 'optics_layer_stack_create failed'
      stop 1
    end if
  end subroutine layer_stack_create

  subroutine layer_stack_add_layer(self, layer)
    class(optics_layer_stack), intent(inout) :: self
    type(optics_layer), intent(in) :: layer
    call optics_check(c_optics_layer_stack_add_layer(self%handle, layer), 'add_layer')
  end subroutine layer_stack_add_layer

  function layer_stack_layer_count(self) result(n)
    class(optics_layer_stack), intent(in) :: self
    integer(c_size_t) :: n
    n = c_optics_layer_stack_layer_count(self%handle)
  end function layer_stack_layer_count

  function layer_stack_get_layer(self, index) result(layer)
    class(optics_layer_stack), intent(in) :: self
    integer, intent(in) :: index
    type(optics_layer) :: layer
    call optics_check(c_optics_layer_stack_get_layer(self%handle, &
      int(index-1, c_size_t), layer), 'get_layer')
  end function layer_stack_get_layer

  subroutine layer_stack_destroy(self)
    class(optics_layer_stack), intent(inout) :: self
    if (c_associated(self%handle)) then
      call c_optics_layer_stack_destroy(self%handle)
      self%handle = c_null_ptr
    end if
  end subroutine layer_stack_destroy

  subroutine layer_stack_final(self)
    type(optics_layer_stack), intent(inout) :: self
    call layer_stack_destroy(self)
  end subroutine layer_stack_final

  ! --- TMM wrappers ---

  subroutine tmm_propagate_direction(stack, crao, angles, n)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: crao
    type(optics_complex), intent(out) :: angles(:)
    integer, intent(inout) :: n
    integer(c_size_t) :: nc
    nc = int(n, c_size_t)
    call optics_check(c_optics_tmm_propagate_direction(stack%handle, crao, angles, nc), &
      'propagate_direction')
    n = int(nc)
  end subroutine tmm_propagate_direction

  subroutine tmm_interface_transfer_matrix_s(stack, propagate_dir, dir_count, wavelength, &
      out_matrices, n)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    real(c_double), intent(in) :: wavelength
    type(optics_matrix2x2), intent(out) :: out_matrices(:)
    integer, intent(inout) :: n
    integer(c_size_t) :: nc
    nc = int(n, c_size_t)
    call optics_check(c_optics_tmm_interface_transfer_matrix_s(stack%handle, propagate_dir, &
      int(dir_count, c_size_t), wavelength, out_matrices, nc), &
      'interface_transfer_matrix_s')
    n = int(nc)
  end subroutine tmm_interface_transfer_matrix_s

  subroutine tmm_interface_transfer_matrix_p(stack, propagate_dir, dir_count, wavelength, &
      out_matrices, n)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    real(c_double), intent(in) :: wavelength
    type(optics_matrix2x2), intent(out) :: out_matrices(:)
    integer, intent(inout) :: n
    integer(c_size_t) :: nc
    nc = int(n, c_size_t)
    call optics_check(c_optics_tmm_interface_transfer_matrix_p(stack%handle, propagate_dir, &
      int(dir_count, c_size_t), wavelength, out_matrices, nc), &
      'interface_transfer_matrix_p')
    n = int(nc)
  end subroutine tmm_interface_transfer_matrix_p

  subroutine tmm_interface_transfer_matrix_with_thickness_s(stack, propagate_dir, dir_count, &
      wavelength, out_matrices, n)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    real(c_double), intent(in) :: wavelength
    type(optics_matrix2x2), intent(out) :: out_matrices(:)
    integer, intent(inout) :: n
    integer(c_size_t) :: nc
    nc = int(n, c_size_t)
    call optics_check(c_optics_tmm_interface_transfer_matrix_with_thickness_s(stack%handle, &
      propagate_dir, int(dir_count, c_size_t), wavelength, out_matrices, nc), &
      'interface_transfer_matrix_with_thickness_s')
    n = int(nc)
  end subroutine tmm_interface_transfer_matrix_with_thickness_s

  subroutine tmm_interface_transfer_matrix_with_thickness_p(stack, propagate_dir, dir_count, &
      wavelength, out_matrices, n)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    real(c_double), intent(in) :: wavelength
    type(optics_matrix2x2), intent(out) :: out_matrices(:)
    integer, intent(inout) :: n
    integer(c_size_t) :: nc
    nc = int(n, c_size_t)
    call optics_check(c_optics_tmm_interface_transfer_matrix_with_thickness_p(stack%handle, &
      propagate_dir, int(dir_count, c_size_t), wavelength, out_matrices, nc), &
      'interface_transfer_matrix_with_thickness_p')
    n = int(nc)
  end subroutine tmm_interface_transfer_matrix_with_thickness_p

  subroutine tmm_last_matrix_s(stack, propagate_dir, dir_count, wavelength, out)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    real(c_double), intent(in) :: wavelength
    type(optics_matrix2x2), intent(out) :: out
    type(optics_matrix2x2) :: buf(16)
    integer :: n
    n = 16
    call tmm_interface_transfer_matrix_with_thickness_s(stack, propagate_dir, dir_count, &
      wavelength, buf, n)
    out = buf(n)
  end subroutine tmm_last_matrix_s

  subroutine tmm_last_matrix_p(stack, propagate_dir, dir_count, wavelength, out)
    type(optics_layer_stack), intent(in) :: stack
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    real(c_double), intent(in) :: wavelength
    type(optics_matrix2x2), intent(out) :: out
    type(optics_matrix2x2) :: buf(16)
    integer :: n
    n = 16
    call tmm_interface_transfer_matrix_with_thickness_p(stack, propagate_dir, dir_count, &
      wavelength, buf, n)
    out = buf(n)
  end subroutine tmm_last_matrix_p

  subroutine tmm_apply_multilayer_effect(matrices, count)
    type(optics_matrix2x2), intent(inout) :: matrices(:)
    integer, intent(in) :: count
    call optics_check(c_optics_tmm_apply_multilayer_effect(matrices, int(count, c_size_t)), &
      'apply_multilayer_effect')
  end subroutine tmm_apply_multilayer_effect

  subroutine tmm_get_structure_pos(stack, out_pos, n)
    type(optics_layer_stack), intent(in) :: stack
    real(c_double), intent(out) :: out_pos(:)
    integer, intent(inout) :: n
    integer(c_size_t) :: nc
    nc = int(n, c_size_t)
    call optics_check(c_optics_tmm_get_structure_pos(stack%handle, out_pos, nc), &
      'get_structure_pos')
    n = int(nc)
  end subroutine tmm_get_structure_pos

  subroutine tmm_find_in_structure(structure_pos, pos_count, depth, out_index, out_depth)
    real(c_double), intent(in) :: structure_pos(:)
    integer, intent(in) :: pos_count
    real(c_double), intent(in) :: depth
    integer, intent(out) :: out_index
    real(c_double), intent(out) :: out_depth
    integer(c_size_t) :: idx
    call optics_check(c_optics_tmm_find_in_structure(structure_pos, int(pos_count, c_size_t), &
      depth, idx, out_depth), 'find_in_structure')
    out_index = int(idx) + 1
  end subroutine tmm_find_in_structure

  subroutine tmm_propagate_matrix_to_depth(wavelength, propagate_dir, dir_count, stack, &
      index, depth_in_structure, out)
    real(c_double), intent(in) :: wavelength
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    type(optics_layer_stack), intent(in) :: stack
    integer, intent(in) :: index
    real(c_double), intent(in) :: depth_in_structure
    type(optics_matrix2x2), intent(out) :: out
    call optics_check(c_optics_tmm_propagate_matrix_to_depth(wavelength, propagate_dir, &
      int(dir_count, c_size_t), stack%handle, int(index-1, c_size_t), depth_in_structure, out), &
      'propagate_matrix_to_depth')
  end subroutine tmm_propagate_matrix_to_depth

  subroutine tmm_get_r_t_from_tmm(tmm, r, t)
    type(optics_matrix2x2), intent(in) :: tmm
    type(optics_complex), intent(out) :: r, t
    call optics_check(c_optics_tmm_get_r_t_from_tmm(tmm, r, t), 'get_r_t_from_tmm')
  end subroutine tmm_get_r_t_from_tmm

  subroutine tmm_get_r_t_power_from_tmm_s(tmm, n1, theta1, n2, theta2, r_power, t_power)
    type(optics_matrix2x2), intent(in) :: tmm
    type(optics_complex), intent(in) :: n1, theta1, n2, theta2
    real(c_double), intent(out) :: r_power, t_power
    call optics_check(c_optics_tmm_get_r_t_power_from_tmm_s(tmm, n1, theta1, n2, theta2, &
      r_power, t_power), 'get_r_t_power_from_tmm_s')
  end subroutine tmm_get_r_t_power_from_tmm_s

  subroutine tmm_get_r_t_power_from_tmm_p(tmm, n1, theta1, n2, theta2, r_power, t_power)
    type(optics_matrix2x2), intent(in) :: tmm
    type(optics_complex), intent(in) :: n1, theta1, n2, theta2
    real(c_double), intent(out) :: r_power, t_power
    call optics_check(c_optics_tmm_get_r_t_power_from_tmm_p(tmm, n1, theta1, n2, theta2, &
      r_power, t_power), 'get_r_t_power_from_tmm_p')
  end subroutine tmm_get_r_t_power_from_tmm_p

  subroutine tmm_get_unpolarized_r_t_power(tmm_s, tmm_p, n1, theta1, n2, theta2, r_power, t_power)
    type(optics_matrix2x2), intent(in) :: tmm_s, tmm_p
    type(optics_complex), intent(in) :: n1, theta1, n2, theta2
    real(c_double), intent(out) :: r_power, t_power
    call optics_check(c_optics_tmm_get_unpolarized_r_t_power(tmm_s, tmm_p, n1, theta1, n2, &
      theta2, r_power, t_power), 'get_unpolarized_r_t_power')
  end subroutine tmm_get_unpolarized_r_t_power

  subroutine tmm_apply_spr_effect(nk, theta, out_theta)
    type(optics_complex), intent(in) :: nk
    type(optics_complex), intent(in) :: theta
    type(optics_complex), intent(out) :: out_theta
    call optics_check(c_optics_tmm_apply_spr_effect(nk, theta, out_theta), 'apply_spr_effect')
  end subroutine tmm_apply_spr_effect

  subroutine tmm_get_e_field_s(e_forward, e_backward, theta, out)
    type(optics_complex), intent(in) :: e_forward, e_backward, theta
    type(optics_vec3), intent(out) :: out
    call optics_check(c_optics_tmm_get_e_field_s(e_forward, e_backward, theta, out), &
      'get_e_field_s')
  end subroutine tmm_get_e_field_s

  subroutine tmm_get_e_field_p(e_forward, e_backward, theta, out)
    type(optics_complex), intent(in) :: e_forward, e_backward, theta
    type(optics_vec3), intent(out) :: out
    call optics_check(c_optics_tmm_get_e_field_p(e_forward, e_backward, theta, out), &
      'get_e_field_p')
  end subroutine tmm_get_e_field_p

  subroutine tmm_get_poynting_vector_s(e_forward, e_backward, theta, n, theta0, n0, out)
    type(optics_complex), intent(in) :: e_forward, e_backward, theta, n, theta0, n0
    real(c_double), intent(out) :: out
    call optics_check(c_optics_tmm_get_poynting_vector_s(e_forward, e_backward, theta, n, &
      theta0, n0, out), 'get_poynting_vector_s')
  end subroutine tmm_get_poynting_vector_s

  subroutine tmm_get_poynting_vector_p(e_forward, e_backward, theta, n, theta0, n0, out)
    type(optics_complex), intent(in) :: e_forward, e_backward, theta, n, theta0, n0
    real(c_double), intent(out) :: out
    call optics_check(c_optics_tmm_get_poynting_vector_p(e_forward, e_backward, theta, n, &
      theta0, n0, out), 'get_poynting_vector_p')
  end subroutine tmm_get_poynting_vector_p

  subroutine tmm_get_absorbed_energy_density_s(e_forward, e_backward, wavelength, &
      propagate_dir, dir_count, stack, index, out)
    type(optics_complex), intent(in) :: e_forward, e_backward
    real(c_double), intent(in) :: wavelength
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    type(optics_layer_stack), intent(in) :: stack
    integer, intent(in) :: index
    real(c_double), intent(out) :: out
    call optics_check(c_optics_tmm_get_absorbed_energy_density_s(e_forward, e_backward, &
      wavelength, propagate_dir, int(dir_count, c_size_t), stack%handle, &
      int(index-1, c_size_t), out), 'get_absorbed_energy_density_s')
  end subroutine tmm_get_absorbed_energy_density_s

  subroutine tmm_get_absorbed_energy_density_p(e_forward, e_backward, wavelength, &
      propagate_dir, dir_count, stack, index, out)
    type(optics_complex), intent(in) :: e_forward, e_backward
    real(c_double), intent(in) :: wavelength
    type(optics_complex), intent(in) :: propagate_dir(:)
    integer, intent(in) :: dir_count
    type(optics_layer_stack), intent(in) :: stack
    integer, intent(in) :: index
    real(c_double), intent(out) :: out
    call optics_check(c_optics_tmm_get_absorbed_energy_density_p(e_forward, e_backward, &
      wavelength, propagate_dir, int(dir_count, c_size_t), stack%handle, &
      int(index-1, c_size_t), out), 'get_absorbed_energy_density_p')
  end subroutine tmm_get_absorbed_energy_density_p

  subroutine tmm_ellipsometric_psi_delta(stack, crao_rad, wavelength, psi, delta)
    type(optics_layer_stack), intent(in) :: stack
    real(c_double), intent(in) :: crao_rad, wavelength
    real(c_double), intent(out) :: psi, delta
    call optics_check(c_optics_tmm_ellipsometric_psi_delta(stack%handle, crao_rad, wavelength, &
      psi, delta), 'ellipsometric_psi_delta')
  end subroutine tmm_ellipsometric_psi_delta

  function interp_nk_quadratic(wl, nk, n, x) result(out_nk)
    real(c_double), intent(in) :: wl(:), x
    type(optics_complex), intent(in) :: nk(:)
    integer, intent(in) :: n
    type(optics_complex) :: out_nk
    real(c_double) :: x0, x1, x2, l0, l1, l2
    integer :: i
    if (n < 3 .or. x <= wl(1)) then
      out_nk = nk(1)
      return
    end if
    if (x >= wl(n)) then
      out_nk = nk(n)
      return
    end if
    do i = 2, n - 1
      if (x >= wl(i-1) .and. x <= wl(i+1)) then
        x0 = wl(i-1)
        x1 = wl(i)
        x2 = wl(i+1)
        l0 = (x - x1)*(x - x2)/((x0 - x1)*(x0 - x2))
        l1 = (x - x0)*(x - x2)/((x1 - x0)*(x1 - x2))
        l2 = (x - x0)*(x - x1)/((x2 - x0)*(x2 - x1))
        out_nk%re = l0*nk(i-1)%re + l1*nk(i)%re + l2*nk(i+1)%re
        out_nk%im = l0*nk(i-1)%im + l1*nk(i)%im + l2*nk(i+1)%im
        return
      end if
    end do
    out_nk = nk(n/2 + 1)
  end function interp_nk_quadratic

end module optics_toolkits
