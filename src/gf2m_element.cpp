#include "gf2m_element.h"

namespace dstu4145
{
    auto gf2m_element::trace() -> gf2m_element
    {
        auto t = *this;
        auto x = *this;

        std::cout << std::endl;

        for (auto i = 1u; i < field_.m(); ++i)
            t = square(t) + x;

        return t;
    }

    auto gf2m_element::half_trace() -> gf2m_element
    {
        auto t = *this;
        auto x = *this;

        for (auto i = 1u; i <= (field_.m() - 1) / 2; ++i)
            t = square(square(t)) + x;

        return t;
    }
}