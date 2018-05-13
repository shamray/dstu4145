#include "gf2m_element.h"

namespace dstu4145
{
    auto gf2m_element::trace() -> gf2m_element
    {
        auto t = *this;

        for (auto i = 1u; i <= field_.m() - 1; ++i)
            t = square(t) + t;

        return t;
    }

    auto gf2m_element::half_trace() -> gf2m_element
    {
        auto t = *this;

        for (auto i = 1u; i <= (field_.m() - 1) / 2; ++i)
            t = square(square(t)) + t;

        return t;
    }
}