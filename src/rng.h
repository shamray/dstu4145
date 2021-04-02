#pragma once

#include "rng_interface.h"
#include "integer.h"

namespace dstu4145
{
    inline auto gen_random_integer(rng_t rng, size_t msb)
    {
        for (;;)
        {
            auto buffer = std::vector<std::byte>{};

            const auto maxsize = msb % 8 ? msb / 8 + 1 : msb / 8;

            while (buffer.size() < maxsize)
                buffer.push_back(rng());

            auto result = integer{ buffer };

            for (auto i = msb; i < buffer.size() * 8; ++i)
                result.bit_unset(i);

            if (result != 0)
                return result;
        }
    }
}