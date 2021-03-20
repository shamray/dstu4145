#pragma once

#include "rng_interface.h"
#include "domain_params.h"
#include "key_pair.h"

#include <vector>

namespace dstu4145
{
	using buffer = std::vector<std::byte>;

    using presignature = std::tuple<integer, gf2m::element>;

    class engine
    {
    public:
        engine(domain_params params);

        auto generate_private_key(rng_t random) const -> private_key;
	    auto generate_key_pair(rng_t random) const -> std::tuple<private_key, public_key>;
        
        auto compute_presignature(rng_t random) const -> presignature;
		auto sign(rng_t random, private_key key, const buffer& hash) const -> buffer;
        
        auto verify(public_key key, const buffer& hash, const buffer& signature) const -> bool;

    private:
		domain_params params_;
    };
}