#include "key_pair.h"
#include "domain_params.h"

namespace dstu4145
{
    private_key::private_key(const std::vector<std::byte> &value)
        : d_{buffer_to_integer(value)}
    {
    }

    public_key::public_key(const domain_params& params, const dstu4145::private_key& value)
        : q_{-(params.p * value)}
    {
    }
}