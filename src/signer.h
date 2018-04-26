#pragma once

#include "rng_interface.h"
#include "domain_params.h"
#include "key_pair.h"

#include <vector>

namespace dstu4145
{
    class signer
    {
    public:
        signer(domain_params params, rng_t random);
        auto sign_hash(std::vector<unsigned char> hash, private_key sign_key) -> std::vector<unsigned char>;

    private:
        auto gen_random_integer() -> integer;

    private:
        domain_params params_;
        rng_t random_;
    };
}