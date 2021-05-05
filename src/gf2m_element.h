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
        auto trace() const -> gf2m_element;
        auto half_trace() const -> gf2m_element;

        auto bit_set(size_t i) { return value_.bit_set(i); }
        auto bit_unset(size_t i) { return value_.bit_unset(i); }
        auto bit_test(size_t i) const { return value_.bit_test(i); }

        template<class iterator>
        void to_buffer(iterator out) const
        {
            value_.to_buffer(out);
        }

    private:
        gf2m field_;
        polynomial value_;
    };

    auto square(const gf2m::element& a) -> gf2m::element;
}