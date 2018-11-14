#pragma once

#include "rng_interface.h"
#include "integer.h"

namespace dstu4145
{
    inline auto gen_random_integer(rng_t rng, unsigned msb)
    {
        auto buffer = std::vector<std::byte>{};

        while(buffer.size() < 32)
            buffer.push_back(rng());

        auto result = buffer_to_integer(buffer);

        for (unsigned i = msb; i < 256; ++i)
            bit_unset(result, i);

        return result;
    }
}