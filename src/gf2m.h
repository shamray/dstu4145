#pragma once

#include "polynomial.h"

namespace dstu4145
{
    class gf2m_element;

    class gf2m: boost::equality_comparable<gf2m>
    {
    public:
        using element = gf2m_element;
        gf2m(int m, int x1, int x2, int x3);
        gf2m(int m, int x);

        auto create_element(integer value) const -> element;
        auto create_element(polynomial value) const -> element;

        auto operator== (const gf2m& other) const  -> bool;

        auto m() const { return m_; }
        auto basis() const { return basis_; }

    private:
        int m_;
        polynomial basis_;
    };
}