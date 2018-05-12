#include "ecurve.h"

namespace dstu4145
{
    auto ecurve::get_point(integer ix, integer iy) const -> point
    {
        return point{*this, ix, iy};
    }
}
