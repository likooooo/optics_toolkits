# C# examples for optics_toolkits

Console programs mirroring the C examples (`examples/c/`). Each writes CSV files under `output/`.

## Prerequisites

- [.NET 8 SDK](https://dotnet.microsoft.com/download)
- Built `liboptics_toolkits.so` (or platform equivalent) from the project CMake build

## Build via CMake

From the repository root:

```bash
cmake -B build -DOPTICS_BUILD_CSHARP=ON
cmake --build build
```

This runs `dotnet build` on the bindings library and all five examples.

## Build manually

```bash
# Build the native shared library first
cmake -B build && cmake --build build

# Build bindings and an example
dotnet build bindings/csharp/OpticsToolkits/OpticsToolkits.csproj -c Release
dotnet build examples/csharp/01_poynting_absorption/01_poynting_absorption.csproj -c Release
```

## Run

The native library must be on the dynamic linker search path. Set `LD_LIBRARY_PATH` to your CMake build directory (where `liboptics_toolkits.so` is produced):

```bash
export LD_LIBRARY_PATH=/path/to/optics_toolkits/build:$LD_LIBRARY_PATH
```

Alternatively, set `OPTICS_TOOLKITS_LIB_PATH` to the directory containing the shared library; the C# bindings use `NativeLibrary` resolution to load from that path.

Run an example from its project directory (CSV files are written to `output/`):

```bash
cd examples/csharp/01_poynting_absorption
dotnet run -c Release
```

Or run the built executable directly:

```bash
LD_LIBRARY_PATH=../../build \
  ./bin/Release/net8.0/01_poynting_absorption
```

## Examples

| Program | Output CSV |
|---------|------------|
| `01_poynting_absorption` | `output/01_poynting_absorption.csv`, `output/01_e_field.csv` |
| `02_reflection_spectrum` | `output/02_reflection_spectrum.csv` |
| `03_transmission_spectrum` | `output/03_transmission_spectrum.csv` |
| `04_ellipsometry` | `output/04_ellipsometry.csv` |
| `05_surface_plasmon_resonance` | `output/05_surface_plasmon_resonance.csv` |
