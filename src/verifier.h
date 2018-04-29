#pragma once

#include "domain_params.h"

namespace dstu4145
{
    class public_key;

    class verifier
    {
    public:
        verifier(domain_params params);
        auto verify_hash(
            const public_key& key,
            std::vector<unsigned char> hash,
            std::vector<unsigned char> signature
        ) -> bool;

    private:
        domain_params params_;
    };
}