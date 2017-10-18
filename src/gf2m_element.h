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
            auto [d, a, b] = extended_euqlid(field_.basis(), value_);
            d; a;
            return element(b);
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

        auto result = integer2{0};
        auto field = a.field_;

        if (b .value_== 0)
          return gf2m::element(field, 0);

        for (int i = boost::multiprecision::msb(b.value_); i>=0; i--)
        {
            result <<= 1;
            //std::cout << i << " : " << boost::multiprecision::bit_test(b.value_, i) << std::endl;
            if (boost::multiprecision::bit_test(b.value_, i))
                result ^= a.value_;
        }

        return field.create_element(static_cast<integer>(modulo(result, field.basis())));
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