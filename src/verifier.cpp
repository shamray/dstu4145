#include "verifier.h"
#include "key_pair.h"

namespace dstu4145
{
    verifier::verifier(domain_params params)
        : params_(std::move(params))
    {}

    auto verifier::verify_hash(
        const public_key& key,
        std::vector<std::byte> hash,
        std::vector<std::byte> signature
    ) -> bool
    {
        const auto& q = static_cast<ecurve::point>(key);
        const auto& p = params_.p;
        const auto& curve = params_.curve;
        const auto& field = curve.field();

        auto h = field.create_element(buffer_to_integer(hash));
        auto s = buffer_to_integer(std::begin(signature), std::begin(signature) + signature.size() / 2);
        auto r = buffer_to_integer(std::begin(signature) + signature.size() / 2, std::end(signature));

        auto rpoint = s * p + r * q;

        auto y = h * rpoint.x;

        return static_cast<dstu4145::integer>(y) == r;
    }
}