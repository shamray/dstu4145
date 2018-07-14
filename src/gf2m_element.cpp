#include "gf2m_element.h"

namespace dstu4145
{
    gf2m_element::gf2m_element(gf2m field, integer value)
        : field_(std::move(field))
        , value_(std::move(value))
    {}

    auto gf2m_element::element(integer value) const -> gf2m_element
    {
        return gf2m_element{field_, std::move(value)};
    }

    auto gf2m_element::inverse() const -> gf2m_element
    {
        auto [d, a, b] = extended_euqlid(field_.basis(), value_, field_.basis());
        d; a;
        return field_.create_element(b);
    }

    auto gf2m_element::trace() -> gf2m_element
    {
        auto t = *this;
        auto x = *this;

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