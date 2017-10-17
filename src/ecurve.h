#pragma once

#include "gf2m.h"
#include "gf2m_element.h"

namespace dstu4145 {

    class ecurve {
        friend struct ecurve_point;
    public:
        using point = ecurve_point;

    public:
        ecurve(gf2m gf, int a, integer b, integer n)
            : gf_(gf)
            , a_(a)
            , b_(b)
            , n_(n)
        {}

    private:
        gf2m gf_;
        int a_;
        integer b_;
        integer n_;
    };

    struct ecurve_point {
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

            auto x = square((p.y + q.y) / (p.x + q.x)) + (p.y + q.y) / (p.x + q.x) + p.x + q.x + gf2m::element{c.gf_, c.a_};
            auto y = (p.y + q.y) / (p.x + q.y) * (p.x + x) + x + p.y;

            return ecurve_point{ c, x, y };
        }

        auto operator* (integer d) const {
            return multiply(d, *this);
        }
        auto operator- () const {
            return ecurve_point{ c, x, x + y };
        }
    };

    inline auto operator* (integer d, ecurve::point p) {
        return p * d;
    }

    inline auto operator== (ecurve::point a, ecurve::point b)
    {
        return a.x == b.x && a.y == b.y;
    }

    inline std::ostream& operator<< (std::ostream& os, const ecurve::point& p)
    {
        return os << std::hex << "(" << p.x << ", " << p.y << ")";
    }
}