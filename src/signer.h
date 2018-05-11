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
        signer(private_key key, domain_params params, rng_t random);
        auto sign_hash(const std::vector<std::byte>& hash) -> std::vector<std::byte>;

    private:
        auto gen_random_integer() -> integer;

    private:
        private_key prvkey_;
        domain_params params_;
        rng_t random_;
    };
}