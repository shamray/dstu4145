#pragma once

#include "integer.h"

namespace dstu4145 {

    class gf2m_element;

    class gf2m: boost::equality_comparable<gf2m> {
    public:
        using element = gf2m_element;
        gf2m(int m, int x1, int x2, int x3);

        auto create_element(integer value) const -> element;

        auto m() const { return m_; }
        auto basis() const { return basis_; }

        auto operator== (const gf2m& other) const {
            return m() == other.m();
        }

    private:
        int m_;
        integer basis_;
    };
}