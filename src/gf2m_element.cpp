#include "gf2m_element.h"
#include "utils.h"

namespace dstu4145
{
    gf2m_element::gf2m_element(gf2m field, integer value)
        : field_(std::move(field))
        , value_(std::move(value))
    {}

    gf2m_element::gf2m_element(gf2m field, polynomial value)
        : field_(std::move(field))
        , value_(std::move(value))
    {}

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

        for (auto i = 1; i < field_.m(); ++i)
            t = square(t) + x;

        return t;
    }

    auto gf2m_element::half_trace() -> gf2m_element
    {
        auto t = *this;
        auto x = *this;

        for (auto i = 1; i <= (field_.m() - 1) / 2; ++i)
            t = square(square(t)) + x;

        return t;
    }

    auto operator==(const gf2m::element& a, const gf2m::element& b) -> bool
    {
        return a.field_ == b.field_ && a.value_ == b.value_;
    }

    auto operator+(const gf2m::element& a, const gf2m::element& b) -> gf2m::element
    {
        if (a.field_ != b.field_)
            throw std::logic_error("invalid operation");

        auto field = a.field_;
        return field.create_element(a.value_ + b.value_);
    }

    auto operator*(const gf2m::element& a, const gf2m::element& b) -> gf2m::element
    {
        if (a.field_ != b.field_)
            throw std::logic_error("invalid operation");

        auto field = a.field_;
        return field.create_element((a.value_* b.value_) % field.basis());
    }

    auto operator/(const gf2m::element& a, const gf2m::element& b) -> gf2m::element
    {
        return a * b.inverse();
    }

    auto square(const gf2m::element& a) -> gf2m::element
    {
        return a * a;
    }

    std::ostream& operator<<(std::ostream& os, const gf2m_element& x)
    {
        return os << std::hex << x.value_;
    }
}