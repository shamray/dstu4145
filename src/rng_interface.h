#pragma once

#include <functional>
#include <cstddef>

namespace dstu4145
{
    using rng_t = std::function<std::byte()>;
}