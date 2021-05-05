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
        : d_{std::move(d)}
    {
    }

    public_key::public_key(const domain_params& params, const std::vector<std::byte>& value)
        : q_{params.curve.expand_point(params.curve.field().create_element(polynomial{value}))}
    {
    }

    public_key::public_key(const domain_params& params, const dstu4145::private_key& priv)
        : q_{-(params.p * priv)}
    {
    }
}