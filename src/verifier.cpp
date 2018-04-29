#include "verifier.h"

namespace dstu4145
{
    verifier::verifier(domain_params params)
        : params_(std::move(params))
    {}

    auto verifier::verify_hash(
        const public_key& key,
        std::vector<unsigned char> hash,
        std::vector<unsigned char> signature
    ) -> bool
    {
        return true;
    }
}