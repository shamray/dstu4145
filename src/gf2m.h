#pragma once

#include "polynomial.h"

namespace dstu4145
{
    class gf2m_element;

    class gf2m: boost::equality_comparable<gf2m>
    {
    public:
        gf2m(int m, int x1, int x2, int x3);
        gf2m(int m, int x);

        auto element(integer value) const -> gf2m_element;
        auto element(polynomial value) const -> gf2m_element;

        auto operator== (const gf2m& other) const  -> bool;

        auto m() const { return m_; }
        auto basis() const -> const auto& { return basis_; }

    private:
        int m_;
        polynomial basis_;
    };
}