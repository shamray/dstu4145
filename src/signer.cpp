#include "signer.h"
#include "rng.h"

#include <map>
#include <vector>

using namespace std::string_literals;

namespace dstu4145
{
    signer::signer(private_key sign_key, domain_params params, rng_t random)
        : prvkey_{std::move(sign_key)}
        , params_{std::move(params)}
        , random_{std::move(random)}
    {

    }

    auto signer::sign_hash(const std::vector<std::byte>& hash, size_t signature_size) -> std::vector<std::byte>
    {
        const auto& curve = params_.curve;
        const auto& field = curve.field();
        const auto& p = params_.p;
        const auto& n = params_.n;

        const auto& d = static_cast<integer>(prvkey_);

        auto e = gen_random_integer(random_, n.msb());

        assert(e < n);
        auto fe = (e * p).x;
        assert(!fe.is_zero());

        auto h = field.create_element(integer{hash});
        assert(!h.is_zero());
        auto y = h * fe;
        auto r = static_cast<integer>(y);
        assert(r != integer{0});

        auto dr = (d * r) % n;
        auto s = (e + dr) % n;
        assert(s != integer{0});

        auto result = std::vector<std::byte>{};

        s.to_buffer(std::back_inserter(result), signature_size / 2);
        r.to_buffer(std::back_inserter(result), signature_size / 2);

        return result;
    }
}