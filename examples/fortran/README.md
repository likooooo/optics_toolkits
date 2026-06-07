# Fortran TMM examples

Five programs mirroring `examples/c/` and the `tmm_examples_1` notebook. Each writes CSV under `output/` (no Python plugins required).

## Build

From the repository root (Fortran bindings enabled by default):

```bash
cmake -S . -B build-f64 \
  -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_C_COMPILER=clang-20 \
  -DCMAKE_PREFIX_PATH=/path/to/infrastructure/.inf_install \
  -DOPTICS_PRECISION=f64
cmake --build build-f64
```

## Run

```bash
cd build-f64/examples/fortran
./01_poynting_absorption
./02_reflection_spectrum
# ...
```

Or via CTest:

```bash
ctest --test-dir build-f64 -L fortran
```

## Requirements

- `gfortran` or Intel `ifx`/`ifort`
- Built `liboptics_toolkits` and `optics_toolkits_fortran` module in the build tree
