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
}