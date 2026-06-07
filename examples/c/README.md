# C TMM examples

Five programs mirroring the `tmm_examples_1` notebook (`examples/c/`). Without Python core plugins, each writes CSV files under `output/` relative to the working directory.

| Program | Output |
|---------|--------|
| `01_poynting_absorption` | `output/01_poynting_absorption.csv`, `output/01_e_field.csv` |
| `02_reflection_spectrum` | `output/02_reflection_spectrum.csv` |
| `03_transmission_spectrum` | `output/03_transmission_spectrum.csv` |
| `04_ellipsometry` | `output/04_ellipsometry.csv` |
| `05_surface_plasmon_resonance` | `output/05_surface_plasmon_resonance.csv` |

## Build

From the repository root:

```bash
cmake -S . -B build-f64 \
  -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_C_COMPILER=clang-20 \
  -DOPTICS_PRECISION=f64 \
  -DCMAKE_PREFIX_PATH=/path/to/infrastructure/.inf_install

cmake --build build-f64
```

## Run

```bash
cd build-f64/examples/c
./01_poynting_absorption
# ...
```

Or via CTest:

```bash
ctest --test-dir build-f64 -L examples
```

With `OPTICS_ENABLE_PYTHON_CORE_PLUGINS=ON` and `core_plugins/` on `PYTHONPATH`, the same binaries plot via matplotlib instead of writing CSV.
