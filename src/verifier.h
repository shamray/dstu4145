#pragma once

#include "domain_params.h"
#include "key_pair.h"

namespace dstu4145
{
    class public_key;

    class verifier
    {
    public:
        verifier(public_key key, domain_params params);
        auto verify_hash(std::vector<std::byte> hash, std::vector<std::byte> signature) -> bool;

    private:
        public_key pubkey_;
        domain_params params_;
    };
}