#pragma once

#include "ecurve.h"

namespace dstu4145
{
    struct domain_params
    {
        ecurve curve;
        integer n;
        ecurve::point p;
    };
}
