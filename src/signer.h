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

        void compute_presignature();
        auto sign_hash(const std::vector<std::byte>& hash) -> std::vector<std::byte>;

    private:
        private_key prvkey_;
        domain_params params_;
        rng_t random_;
    };

    /*
    struct presignature{};
    using buffer = std::vector<std::byte>;

    auto gen_private_key(domain_params, rng_t) -> private_key;
    auto compute_presignature(domain_params, rng_t) -> presignature;
    auto sign_hash(domain_params, private_key, presignature, buffer) -> buffer;
	auto verify_hash(domain_params, public_key, buffer hash, buffer signature) -> bool;
    */
}