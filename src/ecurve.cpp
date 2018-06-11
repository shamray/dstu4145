#include "ecurve.h"

namespace dstu4145
{
    namespace
    {
        auto solve_quadratic_equasion(const gf2m& gf, const gf2m::element& u, const gf2m::element& w) -> std::optional<gf2m::element>
        {
            assert(u != gf.create_element(0));
            assert(w != gf.create_element(0));

            //auto v = w * square(u.inverse());

            auto u1 = u.inverse();
            auto u2 = u1 * u1;
            auto v = w * u2;

            std::cout << std::endl << v << std::endl;
            std::cout << std::endl << v.trace() << std::endl;
            std::cout << std::endl << v.half_trace() << std::endl;

            assert(v.trace() != gf.create_element(1));

            std::cout << std::endl << std::hex << gf.basis() << std::endl;
            return v.half_trace() * u;
//            return gf.create_element(dstu4145::integer{"0x0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"});
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
            //std::cout << std::endl << w << std::endl;
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
