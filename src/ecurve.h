#pragma once

#include "gf2m.h"
#include "gf2m_element.h"
#include "rng_interface.h"

#include <optional>

namespace dstu4145
{
    struct ecurve_point;

    class ecurve
    {
        friend struct ecurve_point;

    public:
        ecurve(gf2m gf, int a, integer b);

        ecurve(ecurve&&) = default;
        ecurve& operator=(ecurve&&) = default;
        ecurve(const ecurve&) = default;
        ecurve& operator=(const ecurve&) = default;

        auto field() const -> const auto& { return gf_; }
        auto a() const { return field().element(a_); }
        auto b() const { return field().element(b_); }

        auto find_point(integer ix) const -> std::optional<ecurve_point>;
        auto find_point(rng_t rng, integer n) const -> ecurve_point;
        auto infinity_point() const -> ecurve_point;

        auto expand_point(gf2m_element compressed) const -> ecurve_point;

    private:
        gf2m gf_;
        int a_;
        integer b_;
    };

    struct ecurve_point
    {
        ecurve_point(ecurve curve);
        ecurve_point(ecurve curve, integer ix, integer iy);
        ecurve_point(ecurve curve, gf2m_element x, gf2m_element y);

        ecurve_point(ecurve_point&&) = default;
        ecurve_point& operator=(ecurve_point&&) = default;
        ecurve_point(const ecurve_point&) = default;
        ecurve_point& operator=(const ecurve_point&) = default;

        gf2m_element x;
        gf2m_element y;
        ecurve c;

        auto operator+(ecurve_point q) const -> ecurve_point;
        auto operator+=(ecurve_point q) -> ecurve_point&;

        auto operator*(integer d) const -> ecurve_point;
        auto operator-() const -> ecurve_point;

        auto validate() const -> bool;
        auto compress() const -> gf2m_element;
    };

    inline auto operator* (const integer& d, const ecurve_point& p)
    {
        return p * d;
    }

    inline auto operator== (const ecurve_point& a, const ecurve_point& b)
    {
        return a.x == b.x && a.y == b.y;
    }

    inline auto operator!= (const ecurve_point& a, const ecurve_point& b)
    {
        return !(a == b);
    }

    inline auto operator<< (std::ostream& os, const ecurve_point& p) -> std::ostream&
    {
        return os << std::hex << "(" << p.x << ", " << p.y << ")";
    }
}