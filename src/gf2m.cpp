#include "gf2m.h"
#include "gf2m_element.h"

namespace dstu4145
{
    gf2m::gf2m(int m, int x1, int x2, int x3)
        : m_(m)
        , basis_(m, x1, x2, x3)
    {
    }

    gf2m::gf2m(int m, int x)
        : m_(m)
        , basis_(m, x)
    {
    }

    auto gf2m::create_element(integer value) const -> element
    {
        return element(*this, value % (integer{ 1 } << m_));
    }

    auto gf2m::create_element(polynomial value) const -> element
    {
        for (auto i = m_; !value.is_zero() && i <= value.msb(); ++i)
            value.bit_unset(i);
        return element(*this, polynomial{value});
    }

    auto gf2m::operator== (const gf2m& other) const  -> bool
    {
        return m() == other.m();
    }
}
