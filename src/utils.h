#pragma once

#include "integer.h"
#include <tuple>

namespace dstu4145 {
    template <class T>
    auto multiply(integer d, const T& p) {
        assert (d != 0);

        if (d == 1)
            return p;

        auto dl = d >> 1;
        auto dr = d - dl;

        auto half = multiply(dl, p);

        if (dl == dr)
          return half + half;
        else
          return half + half + p;
    }

    inline auto p_multiply(integer a, integer b) {
        auto result = integer2{0};

        if (b== 0)
            return result;

        for (int i = boost::multiprecision::msb(b); i>=0; i--)
        {
            result <<= 1;
            if (boost::multiprecision::bit_test(b, i))
                result ^= a;
        }

        return result;

    }

    template <class T, class U>
    auto p_divide(const T &dividend, const U &divisor) {
        U remainder, quotient;

        if (!divisor)
            throw std::logic_error("divide by zero");

        if (dividend == 0)
            return std::make_tuple(remainder, quotient);

        int degree = boost::multiprecision::msb(divisor);

        for (int i=boost::multiprecision::msb(dividend); i>=0; i--)
        {
            remainder <<= 1;
            if (boost::multiprecision::bit_test(dividend, i))
                boost::multiprecision::bit_set(remainder, 0);

            if (boost::multiprecision::bit_test(remainder, degree)) {
                remainder ^= divisor;
                boost::multiprecision::bit_set(quotient, i);
            }
        }

        return std::make_tuple(remainder, quotient);
    }

    template <class T, class U>
    auto p_modulo(const T& a, const U& b) {
        auto[r, q] = p_divide(a, b);
        std::ignore = q;
        return r;
    }

    inline auto extended_euqlid(integer f, integer c, integer mod) {
        if (c == 0)
            return std::make_tuple(f, integer{1}, integer{0});

        auto [r, q] = p_divide(f, c);

        auto[d, a, b] = extended_euqlid(c, r, mod);
        return std::make_tuple(d, b, a ^ p_modulo(p_multiply(q,  b), mod));

       /* auto a = integer{1};
        auto u = integer{1};
        auto d = f;
        auto v = c;

        while(v != 0) {
            auto [r, q] = divide(d, v);
            w = a ^
        }*/
    }
}