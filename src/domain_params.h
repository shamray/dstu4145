#pragma once

#include "ecurve.h"
#include "integer.h"
#include "rng_interface.h"

#include <stdexcept>

namespace dstu4145
{
    struct domain_params
    {
        ecurve curve;
        integer n;
        ecurve_point p;

        // The base point must lie on the curve (checked first: multiplying an
        // off-curve point is meaningless), must not be the point at infinity,
        // and must have order n.
        domain_params(ecurve curve, integer n, ecurve_point p)
            : curve{std::move(curve)}
            , n{std::move(n)}
            , p{std::move(p)}
        {
            const auto o = this->curve.infinity_point();

            if (!this->p.validate() || this->p == o || this->p * this->n != o)
                throw std::runtime_error("invalid domain parameters");
        }

        domain_params(ecurve curve, integer n, rng_t rng)
            : curve{std::move(curve)}
            , n{std::move(n)}
            , p{this->curve.find_point(rng, this->n)}
        {
        }
    };
}
