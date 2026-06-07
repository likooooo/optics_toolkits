# MATLAB examples

Five scripts (`ex_*.m`) mirror the C examples. They require a built MEX gateway and the `+optics` package from `bindings/matlab/`.

## Prerequisites

- MATLAB R2018b or newer with MEX support
- Built `liboptics_toolkits` and `optics_tmm_mex` (`-DOPTICS_BUILD_MATLAB=ON`)

## Build MEX (CMake)

```bash
cmake -S . -B build-f64 \
  -DOPTICS_BUILD_MATLAB=ON \
  -DCMAKE_PREFIX_PATH=/path/to/infrastructure/.inf_install

cmake --build build-f64
```

Add the install or build `matlab` directory (containing `optics_tmm_mex` and `+optics`) to your MATLAB path.

## Run

From this directory in MATLAB:

```matlab
ex_01_poynting_absorption
ex_02_reflection_spectrum
% ...
```

Each script writes CSV under `output/` when Python visualization plugins are not enabled.
