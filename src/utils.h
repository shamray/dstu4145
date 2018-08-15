#pragma once

#include "integer.h"
#include "gf2m_element.h"
#include "gf2m.h"
#include <tuple>

namespace dstu4145
{
    template <class T>
    auto multiply(const integer& d, const T& p)
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

    /*

    inline auto p_multiply(const integer& a, const integer& b)
    {
        auto result = integer{0};

        if (b== 0)
            return result;

        for (long i = boost::multiprecision::msb(b); i>=0; i--)
        {
            result <<= 1;
            if (boost::multiprecision::bit_test(b, static_cast<unsigned>(i)))
                result ^= a;
        }

        return result;
    }

    template <class T, class U>
    auto p_divide(const T &dividend, const U &divisor)
    {
        U remainder, quotient;

        if (!divisor)
            throw std::logic_error("divide by zero");

        if (dividend == 0)
            return std::make_tuple(remainder, quotient);

        long degree = boost::multiprecision::msb(divisor);

        for (long i = boost::multiprecision::msb(dividend); i>=0; i--)
        {
            remainder <<= 1;
            if (boost::multiprecision::bit_test(dividend, static_cast<unsigned>(i)))
                boost::multiprecision::bit_set(remainder, 0);

            if (boost::multiprecision::bit_test(remainder, static_cast<unsigned>(degree))) {
                remainder ^= divisor;
                boost::multiprecision::bit_set(quotient, static_cast<unsigned>(i));
            }
        }

        return std::make_tuple(remainder, quotient);
    }

    template <class T, class U>
    auto p_modulo(const T& a, const U& b)
    {
        auto [r, q] = p_divide(a, b);
        std::ignore = q;
        return r;
    }

    inline auto extended_euqlid(integer f, const integer& c, const integer& mod)
    {
        std::cout << std::hex << f << "\t" << c << "\t" << mod << std::endl;
        if (c == 0)
            return std::make_tuple(f, integer{1}, integer{0});

        auto [r, q] = p_divide(f, c);

        auto [d, a, b] = extended_euqlid(c, r, mod);
        return std::make_tuple(d, b, a ^ p_modulo(p_multiply(q,  b), mod));
    }
    */
}