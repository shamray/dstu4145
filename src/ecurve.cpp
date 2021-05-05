#include "ecurve.h"
#include "rng.h"
#include "arithmetic.h"

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

    ecurve::ecurve(gf2m gf, int a, integer b)
        : gf_{ std::move(gf) }
        , a_{a}
        , b_{std::move(b)}
    {}

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
        return point{*this, std::move(u), std::move(z.value())};
    }

    auto ecurve::find_point(rng_t rng, integer n) const -> point
    {
        for (;;) {
            auto p = find_point(gen_random_integer(rng, field().m()));
            if (p.has_value() && p != infinity_point() && p.value() * n == infinity_point())
                return p.value();
        }
    }

    auto ecurve::infinity_point() const -> point
    {
        return point{*this};
    }

    auto ecurve::expand_point(gf2m_element compressed) const -> point
    {
        auto k = compressed.bit_test(0) ? gf_.create_element(1) : gf_.create_element(0);

        compressed.bit_unset(0);
        auto x = compressed;

        if (compressed.trace() != gf_.create_element(a_))
            x.bit_set(0);

        if (x.is_zero())
            return point{*this};

        auto w = x * x * x + gf_.create_element(a_) * x * x + gf_.create_element(b_);
        if (w.is_zero())
            return point{*this};
    
        auto v = w * square(x.inverse());
        auto z = solve_quadratic_equasion(gf_, gf_.create_element(polynomial{1}), v);
        if (!z.has_value())
            return point{*this};

        if (z.value().trace() == k)
            return point{*this, x, z.value() * x};
        else
            return point{*this, x, (z.value() + gf_.create_element(polynomial{1})) * x};
    }

    ecurve_point::ecurve_point(ecurve curve)
        : x{curve.gf_.create_element(0)}
        , y{curve.gf_.create_element(0)}
        , c{std::move(curve)}
    {}

    ecurve_point::ecurve_point(ecurve curve, integer ix, integer iy)
        : x{curve.gf_, ix}
        , y{curve.gf_, iy}
        , c{std::move(curve)}
    {
        assert(validate());
    }

    ecurve_point::ecurve_point(ecurve curve, gf2m::element x, gf2m::element y)
        : x{std::move(x)}
        , y{std::move(y)}
        , c{std::move(curve)}
    {
        assert(validate());
    }

    auto ecurve_point::operator+(ecurve_point q) const -> ecurve_point
    {
        auto& p = *this;

        if (p == c.infinity_point())
            return q;

        if (q == c.infinity_point())
            return p;

        if (q == -p)
            return c.infinity_point();

        if (p.x == q.x)
        {
            if (p.x.is_zero())
                return c.infinity_point();

            auto t = p.y / p.x + p.x;
            auto x = square(t) + t + gf2m::element{c.gf_, c.a_};

            auto y = square(p.x) + t * x + x;

            auto r = ecurve_point{ c, x, y };
            assert(r.validate());
            return r;
        }
        else
        {
            auto t = (p.y + q.y) / (p.x + q.x);
            auto x = square(t) + t + p.x + q.x + gf2m::element{c.gf_, c.a_};

            auto y = t * (p.x + x) + x + p.y;
            auto r = ecurve_point{ c, x, y };
            assert(r.validate());
            return r;
        }
    }

    auto ecurve_point::operator+=(ecurve_point q) -> ecurve_point&
    {
        *this = *this + q;
        return *this;
    }

    auto ecurve_point::operator*(integer d) const -> ecurve_point
    {
        auto r = multiply(d, *this);
        assert(r.validate());
        return r;
    }

    auto ecurve_point::operator-() const -> ecurve_point
    {
        auto r = ecurve_point{ c, x, x + y };
        assert(r.validate());
        return r;
    }

    auto ecurve_point::validate() const -> bool
    {
        if (*this == c.infinity_point())
            return true;

        return x * x * x + c.field().create_element(c.a()) * x * x + c.field().create_element(c.b()) == y * y + y * x;
    }

    auto ecurve_point::compress() const -> gf2m_element
    {
        if (x.is_zero())
            return c.field().create_element(polynomial{});

        auto i = (y * x.inverse()).trace();

        auto r = x;
        i.bit_test(0) ? r.bit_set(0) : r.bit_unset(0);
        return r;
    }

}
