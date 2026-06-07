#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

if command -v doxygen >/dev/null 2>&1; then
    (cd docs && doxygen Doxyfile)
    echo "Doxygen HTML: docs/build/doxygen/html"
else
    echo "doxygen not found; skipping C API docs"
fi

F90="$ROOT/bindings/fortran/optics_toolkits.f90"
if command -v ford >/dev/null 2>&1; then
    if [ -f "$F90" ]; then
        if ford "$F90"; then
            echo "Ford Fortran docs generated from $F90"
        else
            echo "ford failed; skipping Fortran API docs"
        fi
    else
        echo "Fortran source not found; skipping Ford"
    fi
else
    echo "ford not found; skipping Fortran API docs"
fi

CS_PROJ="$ROOT/bindings/csharp/OpticsToolkits/OpticsToolkits.csproj"
if command -v dotnet >/dev/null 2>&1 && [ -f "$CS_PROJ" ]; then
    if dotnet doc --help >/dev/null 2>&1; then
        OUT="$ROOT/docs/build/csharp"
        mkdir -p "$OUT"
        if dotnet doc generate --project "$CS_PROJ" --output "$OUT"; then
            echo "dotnet doc C# API: $OUT"
        else
            echo "dotnet doc generate failed; skipping C# API docs"
        fi
    else
        echo "dotnet doc not available; skipping C# API docs"
    fi
else
    echo "dotnet not found or C# project missing; skipping C# API docs"
fi

if command -v mkdocs >/dev/null 2>&1; then
    mkdocs build -f docs/mkdocs.yml -d site
    echo "MkDocs site: site/"
else
    echo "mkdocs not found; skipping site build"
fi
