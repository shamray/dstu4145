#pragma once

#include "domain_params.h"

namespace dstu4145
{
    // Curves from DSTU 4145-2002 Appendix G. The numbering matches N in the
    // named curve OID 1.2.804.2.1.1.1.1.3.1.1.2.N.
    enum class curve_id
    {
        uacurve0, // m = 163
        uacurve1, // m = 167
        uacurve2, // m = 173
        uacurve3, // m = 179
        uacurve4, // m = 191
        uacurve5, // m = 233
        uacurve6, // m = 257
        uacurve7, // m = 307
        uacurve8, // m = 367
        uacurve9, // m = 431
    };

    auto standard_curve(curve_id id) -> domain_params;
}
