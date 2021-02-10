#include "key_pair.h"
#include "domain_params.h"
#include "rng.h"

namespace dstu4145
{
    private_key::private_key(const std::vector<std::byte> &value)
        : d_{integer{value}}
    {
    }

    private_key::private_key(integer d)
        : d_(std::move(d))
    {
    }

    public_key::public_key(const domain_params& params, const dstu4145::private_key& priv)
        : q_{-(params.p * priv)}
    {
    }

    auto generate_private_key(const domain_params& params, rng_t random) -> private_key
    {
        auto n = params.n;
		auto d = gen_random_integer(random, n.msb());

        return d;
    }

    auto generate_key_pair(const domain_params& params, rng_t random) -> std::tuple<private_key, public_key>
    {
        auto pri = generate_private_key(params, random);
        auto pub = public_key(params, pri);
        return { pri, pub };
    }
}