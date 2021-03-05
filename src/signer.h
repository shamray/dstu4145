#pragma once

#include "rng_interface.h"
#include "domain_params.h"
#include "key_pair.h"

#include <vector>

namespace dstu4145
{
	using buffer = std::vector<std::byte>;

    class engine
    {
    public:
        engine(domain_params params);

		auto sign(rng_t random, private_key key, const buffer& hash) const -> buffer;
        auto verify(public_key key, const buffer& hash, const buffer& signature) const -> bool;

    private:
		domain_params params_;
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