#include "ecurve.h"

namespace dstu4145
{
    namespace
    {
        auto solve_quadratic_equasion(const gf2m& gf, const gf2m::element& u, const gf2m::element& w) -> std::optional<gf2m::element>
        {
            assert(u != gf.create_element(0));
            assert(w != gf.create_element(0));

            auto v = w * square(u.inverse());

            assert(v.trace() != gf.create_element(1));

            return v.half_trace() * u;
        }
    }
    auto ecurve::find_point(integer ix, integer iy) const -> point
    {
        return point{*this, ix, iy};
    }

    auto ecurve::find_point(integer ix) const -> point
    {
        for (;;) {
            auto u = field().create_element(ix);
            auto w = u * u * u + field().create_element(a()) * u * u + field().create_element(b());
            auto z = solve_quadratic_equasion(field(), u, w);
            if (z.has_value()) {
                //assert(u * u * u + field().create_element(a()) * u * u + field().create_element(b()) == z.value() * z.value() + z.value() * u);
                return point{*this, u, z.value()};
            }
        }
    }

    auto ecurve::infinity_point() const -> point
    {
        return point{*this};
    }
}
