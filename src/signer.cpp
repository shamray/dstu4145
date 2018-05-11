#include "signer.h"

#include <map>

using namespace std::string_literals;

namespace dstu4145
{
    signer::signer(private_key sign_key, domain_params params, rng_t random)
        : prvkey_(std::move(sign_key))
        , params_(std::move(params))
        , random_(std::move(random))
    {

    }

    auto signer::sign_hash(const std::vector<std::byte>& hash) -> std::vector<std::byte>
    {
        const auto& curve = params_.curve;
        const auto& field = curve.field();
        const auto& p = params_.p;
        const auto& n = params_.n;

        const auto& d = static_cast<integer>(prvkey_);

        auto e = gen_random_integer();
        auto fe = (e * p).x;

        auto h = field.create_element(buffer_to_integer(hash));
        auto y = h * fe;
        auto r = static_cast<integer>(y);

        auto dr = static_cast<integer>((static_cast<integer2>(d) * r) % n);
        auto s = (e + dr) % n;

        auto result = std::vector<std::byte>{};

        integer_to_buffer(s, std::back_inserter(result));
        integer_to_buffer(r, std::back_inserter(result));

        return result;
    }

    auto signer::gen_random_integer() -> integer
    {
        auto buffer = std::vector<std::byte>{};

        while(buffer.size() < 32) {
            buffer.push_back(random_());
        }

        return buffer_to_integer(buffer);
    }

}