#pragma once

#include "gf2m.h"
#include "gf2m_element.h"

namespace dstu4145
{
    struct ecurve_point;

    class ecurve
    {
        friend struct ecurve_point;

    public:
        using point = ecurve_point;

    public:
        ecurve(gf2m gf, int a, integer b)
            : gf_(gf)
            , a_(a)
            , b_(b)
        {}

        auto field() const -> const auto& { return gf_; }
        auto a() const { return a_; }
        auto b() const { return b_; }

        auto get_point(integer ix, integer iy) const -> point;

    private:
        gf2m gf_;
        int a_;
        integer b_;
    };

    struct ecurve_point
    {
        ecurve_point(ecurve curve)
            : x(curve.gf_.create_element(0))
            , y(curve.gf_.create_element(0))
            , c(curve)
        {}

        ecurve_point(ecurve curve, integer ix, integer iy)
            : x(curve.gf_, ix)
            , y(curve.gf_, iy)
            , c(curve)
        {}

        ecurve_point(ecurve curve, gf2m::element x, gf2m::element y)
            : x(x)
            , y(y)
            , c(curve)
        {}

        gf2m::element x;
        gf2m::element y;
        ecurve c;

        auto operator+ (ecurve_point q) const
        {
            auto& p = *this;

            if (p.x == q.x)
            {
                auto t = p.y / p.x + p.x;
                auto x = square(t) + t + gf2m::element{c.gf_, c.a_};
                auto y = square(p.x) + t * x + x;

                return ecurve_point{ c, x, y };
            }
            else
            {
                auto t = (p.y + q.y) / (p.x + q.x);
                auto x = square(t) + t + p.x + q.x + gf2m::element{c.gf_, c.a_};
                auto y = t * (p.x + x) + x + p.y;
                return ecurve_point{ c, x, y };
            }
        }

        auto operator*(integer d) const
        {
            return multiply(d, *this);
        }

        auto operator-() const
        {
            return ecurve_point{ c, x, x + y };
        }
    };

    inline auto operator* (integer d, ecurve::point p)
    {
        return p * d;
    }

    inline auto operator== (ecurve::point a, ecurve::point b)
    {
        return a.x == b.x && a.y == b.y;
    }

    inline auto operator!= (ecurve::point a, ecurve::point b)
    {
        return !(a == b);
    }

    inline std::ostream& operator<< (std::ostream& os, const ecurve::point& p)
    {
        return os << std::hex << "(" << p.x << ", " << p.y << ")";
    }
}