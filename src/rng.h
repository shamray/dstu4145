#pragma once

#include "rng_interface.h"
#include "integer.h"

namespace dstu4145
{
    inline auto gen_random_integer(rng_t rng, size_t msb)
    {
        auto buffer = std::vector<std::byte>{};

        while(buffer.size() < 32)
            buffer.push_back(rng());

        auto result = integer{buffer};

        for (auto i = msb; i < 256; ++i)
            result.bit_unset(i);

        return result;
    }
}