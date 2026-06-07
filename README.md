# optics_toolkits

Cross-language C ABI for [simulation](https://github.com/likooooo/simulation) TMM / filmstack.

## Dependencies

- C++26 compiler (e.g. `clang++-20`, GCC 14+)
- [type_traist_notebook](https://github.com/likooooo/type_traist_notebook) — install via sibling [infrastructure](https://github.com/likooooo/infrastructure) into `.inf_install`, then point CMake at that prefix
- `core` git submodule → pinned `simulation` headers (`OPTICS_TOOLKITS_CORE_MIN_REV` in `optics_toolkits.h`)

```bash
git submodule update --init --recursive
```

Optional (`OPTICS_ENABLE_PYTHON_CORE_PLUGINS=ON`): py_visualizer, Boost.Python, Python3, matplotlib.

## Build matrix

Use **separate build directories** per precision; do not reconfigure `OPTICS_PRECISION` in the same tree.

| Option | Default | Notes |
|--------|---------|--------|
| `OPTICS_PRECISION` | `f64` | `f32` or `f64` |
| `OPTICS_BUILD_EXAMPLES` | ON | C examples under `examples/c/` |
| `OPTICS_BUILD_TESTS` | ON | C ABI tests + example CTest entries |
| `OPTICS_BUILD_FORTRAN` | ON | `bindings/fortran` + Fortran examples |
| `OPTICS_BUILD_CSHARP` | ON | .NET 8 bindings + `examples/csharp/` |
| `OPTICS_BUILD_MATLAB` | OFF | MEX when MATLAB is found |
| `OPTICS_ENABLE_PYTHON_CORE_PLUGINS` | OFF | Live plots vs CSV fallback |

Typical local configure (f64, C/Fortran only):

```bash
cmake -S . -B build-f64 \
  -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_C_COMPILER=clang-20 \
  -DOPTICS_PRECISION=f64 \
  -DOPTICS_BUILD_CSHARP=OFF \
  -DCMAKE_PREFIX_PATH=/path/to/infrastructure/.inf_install

cmake --build build-f64
ctest --test-dir build-f64 --output-on-failure
```

## Bindings overview

| Language | Location | Examples |
|----------|----------|----------|
| C (ABI) | `include/optics_toolkits/`, `src/c/` | `examples/c/` — see [README](examples/c/README.md) |
| Fortran | `bindings/fortran/` | `examples/fortran/` — see [README](examples/fortran/README.md) |
| C# | `bindings/csharp/` | `examples/csharp/` — see [README](examples/csharp/README.md) |
| MATLAB | `bindings/matlab/` | `examples/matlab/` — see [README](examples/matlab/README.md) |

All five TMM demos share the same physics scenarios; without Python plugins they emit CSV under `output/`.

## Layout

| Path | Role |
|------|------|
| `include/optics_toolkits/` | Public C headers |
| `src/c/` | `liboptics_toolkits` shared library |
| `core/` | simulation submodule (header-only) |
| `bindings/` | Fortran, C#, MATLAB wrappers |
| `examples/` | Per-language TMM examples |
| `docs/` | Doxygen + MkDocs sources |
| `scripts/` | `check_examples.sh`, `gen_docs.sh` |

## Documentation

From the repo root:

```bash
./scripts/gen_docs.sh
```

- **Doxygen** (if installed): HTML under `docs/build/doxygen/html` from `docs/Doxyfile`
- **MkDocs** (if installed): static site under `site/` from `docs/mkdocs.yml`

Markdown guides: `docs/c-api.md`, `docs/examples.md`, `docs/quickstart.md`.

## CI

GitHub Actions (`.github/workflows/ci.yml`) clones [infrastructure](https://github.com/likooooo/infrastructure), runs `scripts/ci_infra_minimal.sh` (type_traist_notebook + py_visualizer), then:

| Job | What it runs |
|-----|----------------|
| `build-and-test` | f32/f64, C + Fortran + C# examples, `test_tmm_numeric` |
| `viz` | `OPTICS_ENABLE_PYTHON_CORE_PLUGINS=ON`, `ctest -L viz` (`MPLBACKEND=Agg`) |
| `matlab-bindings` | `OPTICS_BUILD_MATLAB=ON` without MATLAB — expects configure skip + static package check |

Private submodules require repository secret `MY_PRIVATE_KEY_1` (SSH deploy key with read access to `simulation`, `type_traist_notebook`, and `py_visualizer`). Configure it under **Settings → Secrets and variables → Actions → Secrets** (not Variables).

Local plugins build:

```bash
cmake -S . -B build-plugins -DOPTICS_ENABLE_PYTHON_CORE_PLUGINS=ON \
  -DCMAKE_CXX_COMPILER=clang++-20 -DCMAKE_PREFIX_PATH=/path/to/infrastructure/.inf_install
cmake --build build-plugins
MPLBACKEND=Agg ctest --test-dir build-plugins -L viz --output-on-failure
```

## Core compatibility

Minimum simulation revision: `63e0f06fe9047cfafe9a687c4677605f648d9ecb` (see `optics_toolkits.h`).
