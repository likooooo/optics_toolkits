# Quickstart

```bash
cmake -S . -B build-f64 \
  -DCMAKE_CXX_COMPILER=clang++-20 \
  -DCMAKE_C_COMPILER=clang-20 \
  -DCMAKE_PREFIX_PATH=/path/to/infrastructure/.inf_install \
  -DOPTICS_PRECISION=f64
cmake --build build-f64
ctest --test-dir build-f64
```
