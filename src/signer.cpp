#include "signer.h"

#include <map>

using namespace std::string_literals;

namespace dstu4145
{
    signer::signer(domain_params params, rng_t random)
        : params_(std::move(params))
        , random_(std::move(random))
    {

    }

    auto signer::sign_hash(const private_key& sign_key, const std::vector<unsigned char>& hash) -> std::vector<unsigned char>
    {
        const auto& curve = params_.curve;
        const auto& field = curve.field();
        const auto& p = params_.p;
        const auto& n = params_.n;

        const auto& d = static_cast<integer>(sign_key);

        auto e = gen_random_integer();
        auto fe = (e * p).x;

        auto h = field.create_element(buffer_to_integer(hash));
        auto y = h * fe;
        auto r = static_cast<integer>(y);

        auto dr = static_cast<integer>((static_cast<integer2>(d) * r) % n);
        auto s = (e + dr) % n;

        auto result = std::vector<unsigned char>{};

        integer_to_buffer(s, std::back_inserter(result));
        integer_to_buffer(r, std::back_inserter(result));

        return result;
    }

    auto signer::gen_random_integer() -> integer
    {
        auto buffer = std::vector<unsigned char>{};

        while(buffer.size() < 32) {
            buffer.push_back(random_());
        }

        return buffer_to_integer(buffer);
    }

}