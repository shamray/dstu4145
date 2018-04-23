#include "gf2m.h"
#include "gf2m_element.h"

namespace dstu4145
{
    gf2m::gf2m(int m, int x1, int x2, int x3)
        : m_(m)
        , basis_(0)
    {
        bit_set(basis_, m);
        bit_set(basis_, x1);
        bit_set(basis_, x2);
        bit_set(basis_, x3);
        bit_set(basis_, 0);
    }

    auto gf2m::create_element(integer value) const -> element {
        return element(*this, value % (integer{ 1 } << m_));
    }
}
