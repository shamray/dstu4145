#pragma once

#include "gf2m.h"

namespace dstu4145
{
    class gf2m_element: boost::equality_comparable<gf2m_element>
    {
    public:
        friend auto operator==(const gf2m::element&, const gf2m::element&) -> bool;
        friend auto operator+(const gf2m::element& a, const gf2m::element& b) -> gf2m::element;
        friend auto operator*(const gf2m::element& a, const gf2m::element& b) -> gf2m::element;
        friend auto operator/(const gf2m::element& a, const gf2m::element& b) -> gf2m::element;
        friend auto operator<<(std::ostream& os, const gf2m_element& x) -> std::ostream&;

        explicit operator integer() { return static_cast<integer>(value_); }

        gf2m_element(gf2m field, integer value);
        gf2m_element(gf2m field, polynomial value);

        gf2m_element(gf2m_element&&) = default;
        gf2m_element& operator=(gf2m_element&&) = default;
        gf2m_element(const gf2m_element&) = default;
        gf2m_element& operator=(const gf2m_element&) = default;

        auto is_zero() const { return value_.is_zero(); }
        auto field() const -> const auto& { return field_; }
        auto inverse() const -> gf2m_element;
        auto trace() -> gf2m_element;
        auto half_trace() -> gf2m_element;

    private:
        gf2m field_;
        polynomial value_;
    };

    auto square(const gf2m::element& a) -> gf2m::element;
}