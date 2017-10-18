#pragma once

#include "integer.h"
#include <tuple>

namespace dstu4145 {
    inline auto extended_euqlid(integer f, integer c) {
        if (c == 0)
            return std::make_tuple(f, integer{0}, integer{1});

        auto[d, a, b] = extended_euqlid(c, f % c);
        return std::make_tuple(d, b, a - f / c * b);
    }

    template <class T>
    auto multiply(integer d, const T& p)
    {
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

    template <class T, class U>
    auto divide(const T &dividend, const U &divisor)
    {
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

            if (boost::multiprecision::bit_test(remainder, degree))
            {
                remainder ^= divisor;
                boost::multiprecision::bit_set(quotient, i);
            }
        }

        return std::make_tuple(remainder, quotient);
    }

    template <class T, class U>
    auto modulo(const T& a, const U& b)
    {
        auto[r, q] = divide(a, b);
        std::ignore = q;
        return r;
    }
}