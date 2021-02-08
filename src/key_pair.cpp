#include "key_pair.h"
#include "domain_params.h"

namespace dstu4145
{
    private_key::private_key(const std::vector<std::byte> &value)
        : d_{integer{value}}
    {
    }

    public_key::public_key(const domain_params& params, const dstu4145::private_key& value)
        : q_{-(params.p * value)}
    {
    }
    auto generate_private_key(const domain_params& params, rng_t random) -> private_key
    {
        return private_key();
    }
    auto generate_key_pair(const domain_params& params, rng_t random) -> std::tuple<private_key, public_key>
    {
        return std::tuple<private_key, public_key>();
    }
}