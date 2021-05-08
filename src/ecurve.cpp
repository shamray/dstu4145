#include "ecurve.h"
#include "rng.h"
#include "arithmetic.h"

namespace dstu4145
{
    namespace
    {
        auto solve_quadratic_equasion(const gf2m& gf, const gf2m_element& u, const gf2m_element& w) -> std::optional<gf2m_element>
        {
            assert(u != gf.element(0));
            assert(w != gf.element(0));

            auto v = w * square(u.inverse());

            if (v.trace() == gf.element(1))
                return std::nullopt;

            return v.half_trace() * u;
        }
    }

    ecurve::ecurve(gf2m gf, int a, integer b)
        : gf_{std::move(gf)}
        , a_{field().element(a)}
        , b_{field().element(std::move(b))}
    {}

    auto ecurve::find_point(integer ix) const -> std::optional<ecurve_point>
    {
        auto u = field().element(std::move(ix));
        if (u == field().element(0))
            return infinity_point();

        auto w = u * u * u + a() * u * u + b();
        auto z = solve_quadratic_equasion(field(), u, w);
        if (!z.has_value())
            return std::nullopt;

        assert(u * u * u + a() * u * u + b() == z.value() * z.value() + z.value() * u);
        return ecurve_point{*this, std::move(u), std::move(z.value())};
    }

    auto ecurve::find_point(rng_t rng, integer n) const -> ecurve_point
    {
        for (;;) {
            auto p = find_point(gen_random_integer(rng, field().m()));
            if (p.has_value() && p != infinity_point() && p.value() * n == infinity_point())
                return p.value();
        }
    }

    auto ecurve::infinity_point() const -> ecurve_point
    {
        return ecurve_point{*this};
    }

    auto ecurve::expand_point(gf2m_element compressed) const -> ecurve_point
    {
        auto k = compressed.bit_test(0) ? field().element(1) : field().element(0);

        compressed.bit_unset(0);
        auto x = compressed;

        if (compressed.trace() != a())
            x.bit_set(0);

        if (x.is_zero())
            return infinity_point();

        auto w = x * x * x + a() * x * x + b();
        if (w.is_zero())
            return infinity_point();
    
        auto v = w * square(x.inverse());
        auto z = solve_quadratic_equasion(field(), field().element(1), v);
        if (!z.has_value())
            return infinity_point();

        if (z.value().trace() == k)
            return ecurve_point{*this, x, z.value() * x};
        else
            return ecurve_point{*this, x, (z.value() + field().element(1)) * x};
    }

    ecurve_point::ecurve_point(ecurve curve)
        : x{curve.field().element(0)}
        , y{curve.field().element(0)}
        , c{std::move(curve)}
    {}

    ecurve_point::ecurve_point(ecurve curve, integer ix, integer iy)
        : x{curve.field(), ix}
        , y{curve.field(), iy}
        , c{std::move(curve)}
    {
        assert(validate());
    }

    ecurve_point::ecurve_point(ecurve curve, gf2m_element x, gf2m_element y)
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
            auto x = square(t) + t + c.a();

            auto y = square(p.x) + t * x + x;

            auto r = ecurve_point{ c, x, y };
            assert(r.validate());
            return r;
        }
        else
        {
            auto t = (p.y + q.y) / (p.x + q.x);
            auto x = square(t) + t + p.x + q.x + c.a();

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

        return x * x * x + c.a() * x * x + c.b() == y * y + y * x;
    }

    auto ecurve_point::compress() const -> gf2m_element
    {
        if (x.is_zero())
            return c.field().element(polynomial{});

        auto i = (y * x.inverse()).trace();

        auto r = x;
        i.bit_test(0) ? r.bit_set(0) : r.bit_unset(0);
        return r;
    }

}
