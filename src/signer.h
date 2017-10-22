#pragma once

#include "rng_interface.h"
#include "domain_params.h"
#include "key_pair.h"

#include <vector>

namespace dstu4145 {
    class signer {
    public:
        signer(domain_params params, rng_t random);
        auto sign_hash(std::vector<char> hash, private_key sign_key) -> std::vector<char>;
    };
}