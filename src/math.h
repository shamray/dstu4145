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
        auto addend = shift(p,d.lsb());
        auto result = addend;

        for (auto i = d.lsb() + 1; i <= d.msb(); ++i) {
            addend += addend;
            if (d.bit_test(i))
                result = result + addend;
        }

        return result;
    }
} 