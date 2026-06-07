#include <optics_toolkits/optics_tmm.h>

#include <filmstack/transfer_matrix_methord.hpp>

#include "internal/exception_bridge.hpp"
#include "internal/precision.hpp"

#include <cmath>
#include <stdexcept>
#include <vector>

namespace {

using optics::internal::cT;
using optics::internal::guard;
using optics::internal::rT;
using optics::internal::to_api;
using optics::internal::to_internal;

using Meterial = typename film_stack_solver<rT>::meterial;

struct LayerStackImpl {
    std::vector<Meterial> layers;
};

inline LayerStackImpl* as_impl(optics_layer_stack* stack)
{
    return reinterpret_cast<LayerStackImpl*>(stack);
}

inline const LayerStackImpl* as_impl(const optics_layer_stack* stack)
{
    return reinterpret_cast<const LayerStackImpl*>(stack);
}

std::vector<Meterial> layers_from_stack(const optics_layer_stack* stack)
{
    if (!stack)
        throw std::invalid_argument("stack is null");
    return as_impl(stack)->layers;
}

std::vector<cT> angles_from_api(const optics_complex* dir, size_t count)
{
    std::vector<cT> out(count);
    for (size_t i = 0; i < count; ++i)
        out[i] = to_internal(dir[i]);
    return out;
}

void require_buffer(size_t needed, size_t capacity)
{
    if (capacity < needed)
        throw std::invalid_argument("output buffer too small");
}

template<char pol>
using Tmm = transfer_matrix_method<rT, pol>;

} // namespace

extern "C" {

optics_layer_stack* optics_layer_stack_create(void)
{
    try {
        return reinterpret_cast<optics_layer_stack*>(new LayerStackImpl{});
    } catch (...) {
        return nullptr;
    }
}

optics_status optics_layer_stack_add_layer(optics_layer_stack* stack, optics_layer layer)
{
    return guard([&] {
        if (!stack)
            throw std::invalid_argument("stack is null");
        as_impl(stack)->layers.push_back(Meterial{to_internal(layer.nk), static_cast<rT>(layer.depth)});
    });
}

size_t optics_layer_stack_layer_count(const optics_layer_stack* stack)
{
    return stack ? as_impl(stack)->layers.size() : 0;
}

optics_status optics_layer_stack_get_layer(const optics_layer_stack* stack, size_t index, optics_layer* out)
{
    return guard([&] {
        if (!stack || !out)
            throw std::invalid_argument("null argument");
        const auto* impl = as_impl(stack);
        if (index >= impl->layers.size())
            throw std::invalid_argument("layer index out of range");
        const auto& m = impl->layers[index];
        *out = optics_layer{to_api(m.nk), static_cast<double>(m.depth)};
    });
}

void optics_layer_stack_destroy(optics_layer_stack* stack)
{
    delete as_impl(stack);
}

optics_status optics_tmm_propagate_direction(
    const optics_layer_stack* stack,
    optics_complex crao,
    optics_complex* out_angles,
    size_t* out_count)
{
    return guard([&] {
        const auto nk = layers_from_stack(stack);
        if (nk.empty())
            throw std::invalid_argument("empty layer stack");
        auto dir = TMM_helper<rT>::propagate_direction(nk, to_internal(crao));
        require_buffer(dir.size(), out_count ? *out_count : 0);
        if (!out_angles || !out_count)
            throw std::invalid_argument("null output");
        for (size_t i = 0; i < dir.size(); ++i)
            out_angles[i] = to_api(dir[i]);
        *out_count = dir.size();
    });
}

static optics_status interface_transfer_matrix_impl(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count,
    bool with_thickness,
    char pol)
{
    return guard([&] {
        const auto nk = layers_from_stack(stack);
        auto dir = angles_from_api(propagate_dir, dir_count);
        if (dir.size() != nk.size())
            throw std::invalid_argument("propagate_dir size mismatch");

        std::vector<matrix2x2<cT>> mats;
        const rT lambda = static_cast<rT>(wavelength);
        if (pol == 's') {
            mats = with_thickness
                ? Tmm<'s'>::interface_transfer_matrix_with_thickness(nk, dir, lambda)
                : Tmm<'s'>::interface_transfer_matrix_with_thickness_without_multiply(nk, dir, lambda);
        } else {
            mats = with_thickness
                ? Tmm<'p'>::interface_transfer_matrix_with_thickness(nk, dir, lambda)
                : Tmm<'p'>::interface_transfer_matrix_with_thickness_without_multiply(nk, dir, lambda);
        }

        require_buffer(mats.size(), out_count ? *out_count : 0);
        if (!out_matrices || !out_count)
            throw std::invalid_argument("null output");
        for (size_t i = 0; i < mats.size(); ++i)
            out_matrices[i] = to_api(mats[i]);
        *out_count = mats.size();
    });
}

optics_status optics_tmm_interface_transfer_matrix_s(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count)
{
    return interface_transfer_matrix_impl(stack, propagate_dir, dir_count, wavelength, out_matrices, out_count, false, 's');
}

optics_status optics_tmm_interface_transfer_matrix_p(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count)
{
    return interface_transfer_matrix_impl(stack, propagate_dir, dir_count, wavelength, out_matrices, out_count, false, 'p');
}

optics_status optics_tmm_interface_transfer_matrix_with_thickness_s(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count)
{
    return interface_transfer_matrix_impl(stack, propagate_dir, dir_count, wavelength, out_matrices, out_count, true, 's');
}

optics_status optics_tmm_interface_transfer_matrix_with_thickness_p(
    const optics_layer_stack* stack,
    const optics_complex* propagate_dir,
    size_t dir_count,
    double wavelength,
    optics_matrix2x2* out_matrices,
    size_t* out_count)
{
    return interface_transfer_matrix_impl(stack, propagate_dir, dir_count, wavelength, out_matrices, out_count, true, 'p');
}

optics_status optics_tmm_apply_multilayer_effect(optics_matrix2x2* matrices, size_t count)
{
    return guard([&] {
        if (!matrices && count > 0)
            throw std::invalid_argument("null matrices");
        std::vector<matrix2x2<cT>> m(count);
        for (size_t i = 0; i < count; ++i)
            m[i] = to_internal(matrices[i]);
        m = TMM_helper<rT>::apply_multilayer_effect(std::move(m));
        for (size_t i = 0; i < count; ++i)
            matrices[i] = to_api(m[i]);
    });
}

optics_status optics_tmm_get_structure_pos(
    const optics_layer_stack* stack,
    double* out_pos,
    size_t* out_count)
{
    return guard([&] {
        const auto nk = layers_from_stack(stack);
        auto pos = TMM_helper<rT>::get_structure_pos(nk);
        require_buffer(pos.size(), out_count ? *out_count : 0);
        if (!out_pos || !out_count)
            throw std::invalid_argument("null output");
        for (size_t i = 0; i < pos.size(); ++i)
            out_pos[i] = static_cast<double>(pos[i]);
        *out_count = pos.size();
    });
}

optics_status optics_tmm_find_in_structure(
    const double* structure_pos,
    size_t pos_count,
    double depth,
    size_t* out_index,
    double* out_depth_in_structure)
{
    return guard([&] {
        if (!structure_pos || !out_index || !out_depth_in_structure)
            throw std::invalid_argument("null argument");
        std::vector<rT> pos(pos_count);
        for (size_t i = 0; i < pos_count; ++i)
            pos[i] = static_cast<rT>(structure_pos[i]);
        auto [index, depth_in] = TMM_helper<rT>::find_in_structure(pos, static_cast<rT>(depth));
        *out_index = index;
        *out_depth_in_structure = static_cast<double>(depth_in);
    });
}

optics_status optics_tmm_propagate_matrix_to_depth(
    double wavelength,
    const optics_complex* propagate_dir,
    size_t dir_count,
    const optics_layer_stack* stack,
    size_t index,
    double depth_in_structure,
    optics_matrix2x2* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        const auto nk = layers_from_stack(stack);
        auto dir = angles_from_api(propagate_dir, dir_count);
        auto m = TMM_helper<rT>::propagate_matrix_to_target_depth_v1(
            static_cast<rT>(wavelength), dir, nk, index, static_cast<rT>(depth_in_structure));
        *out = to_api(m);
    });
}

optics_status optics_tmm_get_r_t_from_tmm(
    optics_matrix2x2 tmm,
    optics_complex* out_r,
    optics_complex* out_t)
{
    return guard([&] {
        if (!out_r || !out_t)
            throw std::invalid_argument("null output");
        auto [r, t] = TMM_helper<rT>::get_r_t_from_tmm(to_internal(tmm));
        *out_r = to_api(r);
        *out_t = to_api(t);
    });
}

optics_status optics_tmm_get_r_t_power_from_tmm_s(
    optics_matrix2x2 tmm,
    optics_complex n1,
    optics_complex theta1,
    optics_complex n2,
    optics_complex theta2,
    double* out_r_power,
    double* out_t_power)
{
    return guard([&] {
        if (!out_r_power || !out_t_power)
            throw std::invalid_argument("null output");
        auto [r, t] = Tmm<'s'>::get_r_t_power_from_tmm(
            to_internal(tmm), to_internal(n1), to_internal(theta1), to_internal(n2), to_internal(theta2));
        *out_r_power = static_cast<double>(r);
        *out_t_power = static_cast<double>(t);
    });
}

optics_status optics_tmm_get_r_t_power_from_tmm_p(
    optics_matrix2x2 tmm,
    optics_complex n1,
    optics_complex theta1,
    optics_complex n2,
    optics_complex theta2,
    double* out_r_power,
    double* out_t_power)
{
    return guard([&] {
        if (!out_r_power || !out_t_power)
            throw std::invalid_argument("null output");
        auto [r, t] = Tmm<'p'>::get_r_t_power_from_tmm(
            to_internal(tmm), to_internal(n1), to_internal(theta1), to_internal(n2), to_internal(theta2));
        *out_r_power = static_cast<double>(r);
        *out_t_power = static_cast<double>(t);
    });
}

optics_status optics_tmm_get_unpolarized_r_t_power(
    optics_matrix2x2 tmm_s,
    optics_matrix2x2 tmm_p,
    optics_complex n1,
    optics_complex theta1,
    optics_complex n2,
    optics_complex theta2,
    double* out_r_power,
    double* out_t_power)
{
    return guard([&] {
        if (!out_r_power || !out_t_power)
            throw std::invalid_argument("null output");
        auto [r, t] = TMM_helper<rT>::get_unpolarized_r_t_power_from_tmm(
            to_internal(tmm_s), to_internal(tmm_p),
            to_internal(n1), to_internal(theta1), to_internal(n2), to_internal(theta2));
        *out_r_power = static_cast<double>(r);
        *out_t_power = static_cast<double>(t);
    });
}

optics_status optics_tmm_apply_spr_effect(
    optics_complex nk,
    optics_complex theta,
    optics_complex* out_theta)
{
    return guard([&] {
        if (!out_theta)
            throw std::invalid_argument("null output");
        *out_theta = to_api(TMM_helper<rT>::apply_surface_plasmon_resonance_effect(to_internal(nk), to_internal(theta)));
    });
}

optics_status optics_tmm_get_e_field_s(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_vec3* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        auto v = Tmm<'s'>::get_e_field(to_internal(e_forward), to_internal(e_backward), to_internal(theta));
        out->x = to_api(v[0]);
        out->y = to_api(v[1]);
        out->z = to_api(v[2]);
    });
}

optics_status optics_tmm_get_e_field_p(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_vec3* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        auto v = Tmm<'p'>::get_e_field(to_internal(e_forward), to_internal(e_backward), to_internal(theta));
        out->x = to_api(v[0]);
        out->y = to_api(v[1]);
        out->z = to_api(v[2]);
    });
}

optics_status optics_tmm_get_poynting_vector_s(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_complex n,
    optics_complex theta0,
    optics_complex n0,
    double* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        *out = static_cast<double>(Tmm<'s'>::get_poynting_vector(
            to_internal(e_forward), to_internal(e_backward),
            to_internal(theta), to_internal(n), to_internal(theta0), to_internal(n0)));
    });
}

optics_status optics_tmm_get_poynting_vector_p(
    optics_complex e_forward,
    optics_complex e_backward,
    optics_complex theta,
    optics_complex n,
    optics_complex theta0,
    optics_complex n0,
    double* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        *out = static_cast<double>(Tmm<'p'>::get_poynting_vector(
            to_internal(e_forward), to_internal(e_backward),
            to_internal(theta), to_internal(n), to_internal(theta0), to_internal(n0)));
    });
}

optics_status optics_tmm_get_absorbed_energy_density_s(
    optics_complex e_forward,
    optics_complex e_backward,
    double wavelength,
    const optics_complex* propagate_dir,
    size_t dir_count,
    const optics_layer_stack* stack,
    size_t index,
    double* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        const auto nk = layers_from_stack(stack);
        auto dir = angles_from_api(propagate_dir, dir_count);
        *out = static_cast<double>(Tmm<'s'>::get_absorbed_energy_density(
            to_internal(e_forward), to_internal(e_backward),
            static_cast<rT>(wavelength), dir, nk, index));
    });
}

optics_status optics_tmm_get_absorbed_energy_density_p(
    optics_complex e_forward,
    optics_complex e_backward,
    double wavelength,
    const optics_complex* propagate_dir,
    size_t dir_count,
    const optics_layer_stack* stack,
    size_t index,
    double* out)
{
    return guard([&] {
        if (!out)
            throw std::invalid_argument("null output");
        const auto nk = layers_from_stack(stack);
        auto dir = angles_from_api(propagate_dir, dir_count);
        *out = static_cast<double>(Tmm<'p'>::get_absorbed_energy_density(
            to_internal(e_forward), to_internal(e_backward),
            static_cast<rT>(wavelength), dir, nk, index));
    });
}

optics_status optics_tmm_ellipsometric_psi_delta(
    const optics_layer_stack* stack,
    double crao_rad,
    double wavelength,
    double* out_psi,
    double* out_delta)
{
    return guard([&] {
        if (!out_psi || !out_delta)
            throw std::invalid_argument("null output");
        const auto nk = layers_from_stack(stack);
        auto dir = TMM_helper<rT>::propagate_direction(nk, cT(static_cast<rT>(crao_rad), rT(0)));
        const rT lambda = static_cast<rT>(wavelength);

        auto tmm_s = Tmm<'s'>::interface_transfer_matrix_with_thickness(nk, dir, lambda);
        auto tmm_p = Tmm<'p'>::interface_transfer_matrix_with_thickness(nk, dir, lambda);
        auto [rs, ts] = TMM_helper<rT>::get_r_t_from_tmm(tmm_s.back());
        auto [rp, tp] = TMM_helper<rT>::get_r_t_from_tmm(tmm_p.back());
        (void)ts;
        (void)tp;

        const cT ratio = rp / rs;
        *out_psi = static_cast<double>(std::atan(std::abs(ratio)));
        *out_delta = static_cast<double>(std::arg(-ratio));
    });
}

optics_status optics_tmm_compute_forward_backward_at_depth(
    const optics_layer_stack* stack,
    double crao_rad,
    double wavelength,
    char polarization,
    double depth,
    optics_complex* out_forward,
    optics_complex* out_backward)
{
    return guard([&] {
        if (!out_forward || !out_backward)
            throw std::invalid_argument("null output");
        if (polarization != 's' && polarization != 'p')
            throw std::invalid_argument("polarization must be 's' or 'p'");

        const auto nk = layers_from_stack(stack);
        auto dir = TMM_helper<rT>::propagate_direction(nk, cT(static_cast<rT>(crao_rad), rT(0)));
        const rT lambda = static_cast<rT>(wavelength);

        std::vector<matrix2x2<cT>> iface;
        if (polarization == 's')
            iface = Tmm<'s'>::interface_transfer_matrix_with_thickness_without_multiply(nk, dir, lambda);
        else
            iface = Tmm<'p'>::interface_transfer_matrix_with_thickness_without_multiply(nk, dir, lambda);

        auto tmm_list = TMM_helper<rT>::apply_multilayer_effect(iface);
        auto [r, t] = TMM_helper<rT>::get_r_t_from_tmm(tmm_list.back());

        cT vw_forward = t;
        cT vw_backward = cT(0);

        auto pos = TMM_helper<rT>::get_structure_pos(nk);
        auto [index, depth_in] = TMM_helper<rT>::find_in_structure(pos, static_cast<rT>(depth));

        for (int i = static_cast<int>(iface.size()) - 1; i >= 0; --i) {
            const auto& M = iface[static_cast<size_t>(i)];
            cT next_f = M[0][0] * vw_forward + M[0][1] * vw_backward;
            cT next_b = M[1][0] * vw_forward + M[1][1] * vw_backward;
            vw_forward = next_f;
            vw_backward = next_b;
        }

        auto phase_m = TMM_helper<rT>::propagate_matrix_to_target_depth_v1(
            lambda, dir, nk, index, depth_in);
        const cT phased_f = phase_m[0][0] * vw_forward + phase_m[0][1] * vw_backward;
        const cT phased_b = phase_m[1][0] * vw_forward + phase_m[1][1] * vw_backward;
        vw_forward = phased_f;
        vw_backward = phased_b;

        *out_forward = to_api(vw_forward);
        *out_backward = to_api(vw_backward);
    });
}

} // extern "C"
