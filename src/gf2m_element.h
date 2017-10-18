#pragma once

#include "gf2m.h"
#include "utils.h"

namespace dstu4145 {

    class gf2m_element {
    public:
        friend auto operator== (const gf2m::element&, const gf2m::element&) -> bool;
        friend auto operator+ (const gf2m::element& a, const gf2m::element& b) -> gf2m::element;
        friend auto operator* (const gf2m::element& a, const gf2m::element& b) -> gf2m::element;
        friend auto operator/ (const gf2m::element& a, const gf2m::element& b) -> gf2m::element;
        friend auto operator<< (std::ostream& os, const gf2m_element& x) -> std::ostream&;

        gf2m_element(const gf2m& field, integer value)
            : field_(field)
            , value_(value)
        {}

        auto element(integer value) const { return gf2m_element{field_, value}; }

        operator integer() {
            return value_;
        }

        auto inverse() const {
            auto [d, a, b] = extended_euqlid(field_.basis(), value_, field_.basis());
            d; a;
            return field_.create_element(b);
        }

    private:
        gf2m field_;
        integer value_;
    };

    inline auto operator== (const gf2m::element& a, const gf2m::element& b) -> bool {
        return a.field_ == b.field_ && a.value_ == b.value_;
    }

    inline auto operator+ (const gf2m::element& a, const gf2m::element& b) -> gf2m::element {
        if (a.field_ != b.field_)
            throw std::logic_error("invalid operation");

        auto field = a.field_;
        auto result = a.value_ ^ b.value_;

        return field.create_element(result);
    }

    inline auto operator* (const gf2m::element& a, const gf2m::element& b) -> gf2m::element {
        if (a.field_ != b.field_)
            throw std::logic_error("invalid operation");

        auto field = a.field_;
        auto result = p_multiply(a.value_, b.value_);

        return field.create_element(static_cast<integer>(p_modulo(result, field.basis())));
    }

    inline auto operator/ (const gf2m::element& a, const gf2m::element& b) -> gf2m::element {
        return a * b.inverse();
    }

    inline auto square (const gf2m::element& a) {
        return a * a;
    }

    inline std::ostream& operator<< (std::ostream& os, const gf2m_element& x)
    {
        return os << std::hex << x.value_;
    }
}