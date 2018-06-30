#pragma once

#include "ecurve.h"
#include "integer.h"
#include "rng_interface.h"

namespace dstu4145
{
    struct domain_params
    {
        ecurve curve;
        integer n;
        ecurve::point p;

        domain_params(ecurve curve, integer n, ecurve::point p)
            : curve{(curve)}
            , n{n}
            , p{p}
        {}

        domain_params(ecurve curve, integer n, rng_t rng)
            : curve{std::move(curve)}
            , n{std::move(n)}
            , p{this->curve.find_point(rng)}
        {
        }
    };
}
