#pragma once

#include "domain_params.h"

namespace dstu4145
{
    class verifier
    {
        verifier(domain_params params);
        auto verify_hash(std::vector<char> hash, std::vector<char> signature) -> bool;
    };
}