#include "signer.h"
#include "rng.h"

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

        auto e = gen_random_integer(random_, boost::multiprecision::msb(n)-1);
        assert(e < n);
        eee = e;
        auto fe = (e * p).x;
        assert(!fe.is_zero());

        auto h = field.create_element(buffer_to_integer(hash));
        assert(!h.is_zero());
        auto y = h * fe;
        auto r = static_cast<integer>(y);
        assert(r != integer{0});

        auto dr = (d * r) % n;
        auto s = (e + dr) % n;
        assert(s != integer{0});

        auto result = std::vector<std::byte>{};

        integer_to_buffer(s, std::back_inserter(result));
        integer_to_buffer(r, std::back_inserter(result));

        return result;
    }
}