#pragma once

#include <optics_toolkits/optics_types.h>

#include <type_traist_notebook/numerics.hpp>
#include <type_traist_notebook/type_traist.hpp>

#include <complex>

#ifndef OPTICS_REAL_TYPE
#define OPTICS_REAL_TYPE double
#endif

namespace optics::internal {

using rT = OPTICS_REAL_TYPE;
using cT = std::complex<rT>;

inline cT to_internal(const optics_complex& z)
{
    return cT(static_cast<rT>(z.re), static_cast<rT>(z.im));
}

inline optics_complex to_api(cT z)
{
    return optics_complex{static_cast<double>(z.real()), static_cast<double>(z.imag())};
}

inline optics_matrix2x2 to_api(const matrix2x2<cT>& m)
{
    optics_matrix2x2 out{};
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            out.m[i][j] = to_api(m[i][j]);
    return out;
}

inline matrix2x2<cT> to_internal(optics_matrix2x2 m)
{
    matrix2x2<cT> out{};
    for (int i = 0; i < 2; ++i)
        for (int j = 0; j < 2; ++j)
            out[i][j] = to_internal(m.m[i][j]);
    return out;
}

} // namespace optics::internal
