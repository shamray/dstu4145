#pragma once

#include "rng_interface.h"
#include "integer.h"

namespace dstu4145
{
    inline auto gen_random_integer(rng_t rng)
    {
        auto buffer = std::vector<std::byte>{};

        while(buffer.size() < 32)
            buffer.push_back(rng());

        return buffer_to_integer(buffer);
    }
}