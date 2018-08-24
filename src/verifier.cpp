#include "verifier.h"

namespace dstu4145
{
    verifier::verifier(public_key key, domain_params params)
        : pubkey_(std::move(key))
        , params_(std::move(params))
    {}

    auto verifier::verify_hash(std::vector<std::byte> hash, std::vector<std::byte> signature) -> bool
    {
        const auto& q = static_cast<ecurve::point>(pubkey_);
        const auto& p = params_.p;
        const auto& curve = params_.curve;
        const auto& field = curve.field();

        auto h = field.create_element(buffer_to_integer(hash));
        auto s = buffer_to_integer(std::begin(signature), std::begin(signature) + signature.size() / 2);
        auto r = buffer_to_integer(std::begin(signature) + signature.size() / 2, std::end(signature));

        std::cout << std::hex << std::endl << "r= " << r << std::endl;
        std::cout << std::hex << std::endl << "s= " << s << std::endl;

        auto rpoint = s * p + r * q;

        auto y = h * rpoint.x;

        std::cout << std::hex << std::endl << "r`=" << static_cast<dstu4145::integer>(y) << std::endl;

        return static_cast<dstu4145::integer>(y) == r;
    }
}