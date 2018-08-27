#include "ecurve.h"
#include "rng.h"

namespace dstu4145
{
    namespace
    {
        auto solve_quadratic_equasion(const gf2m& gf, const gf2m::element& u, const gf2m::element& w) -> std::optional<gf2m::element>
        {
            assert(u != gf.create_element(0));
            assert(w != gf.create_element(0));

            auto v = w * square(u.inverse());

            if (v.trace() == gf.create_element(1))
                return std::nullopt;

            return v.half_trace() * u;
        }
    }

    auto ecurve::find_point(integer ix) const -> std::optional<point>
    {
        auto u = field().create_element(std::move(ix));
        if (u == field().create_element(0))
            return infinity_point();

        auto w = u * u * u + field().create_element(a()) * u * u + field().create_element(b());
        auto z = solve_quadratic_equasion(field(), u, w);
        if (!z.has_value())
            return std::nullopt;

        assert(u * u * u + field().create_element(a()) * u * u + field().create_element(b()) == z.value() * z.value() + z.value() * u);
        return point{*this, u, z.value()};
    }

    auto ecurve::find_point(rng_t rng, integer n) const -> point
    {
        for (;;) {
            auto p = find_point(gen_random_integer(rng, field().m() - 1));
            if (p.has_value() && p.value() * n != infinity_point())
                return p.value();
        }
    }

    auto ecurve::infinity_point() const -> point
    {
        return point{*this};
    }

    auto ecurve_point::validate() const -> bool
    {
        if (*this == c.infinity_point())
            return true;

        return x * x * x + c.field().create_element(c.a()) * x * x + c.field().create_element(c.b()) == y * y + y * x;
    }

}
