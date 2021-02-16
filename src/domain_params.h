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
            : curve{std::move(curve)}
            , n{std::move(n)}
            , p{std::move(p)}
        {
            assert(this->p != this->curve.infinity_point());
            assert(this->p * this->n == this->curve.infinity_point());
        }

        domain_params(ecurve curve, integer n, rng_t rng)
            : curve{std::move(curve)}
            , n{std::move(n)}
            , p{this->curve.find_point(rng, this->n)}
        {
        }
    };
}
