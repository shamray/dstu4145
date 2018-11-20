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
        auto shift = [](auto x, auto y) {
            for (auto i = 0u; i < y; ++i)
                x += x;
            return x;
        };
        auto addend = shift(p,boost::multiprecision::lsb(d));
        auto result = addend;

        for (auto i = boost::multiprecision::lsb(d) + 1; i <= boost::multiprecision::msb(d); ++i) {
            addend += addend;
            if (bit_test(d, i))
                result = result + addend;
        }

        return result;
    }
} 