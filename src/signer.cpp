#include "signer.h"
#include "rng.h"

#include <map>
#include <vector>

using namespace std::string_literals;

namespace dstu4145
{
    engine::engine(domain_params params)
		: params_{ std::move(params) }
    {
    }

    auto engine::sign(rng_t random, private_key key, const buffer& hash) const -> buffer
    {
		const auto& curve = params_.curve;
		const auto& field = curve.field();
		const auto& p = params_.p;
		const auto& n = params_.n;

		const auto signature_size = n.size_in_bytes() * 16;

		const auto& d = static_cast<integer>(key);

		auto e = gen_random_integer(random, n.msb());

		assert(e < n);
		auto fe = (e * p).x;
		assert(!fe.is_zero());

		auto h = field.create_element(integer{ hash });
		assert(!h.is_zero());
		auto y = h * fe;
		auto r = static_cast<integer>(y);
		assert(r != integer{ 0 });

		auto dr = (d * r) % n;
		auto s = (e + dr) % n;
		assert(s != integer{ 0 });

		auto result = std::vector<std::byte>{};

		s.to_buffer(std::back_inserter(result), signature_size / 2);
		r.to_buffer(std::back_inserter(result), signature_size / 2);

		return result;
    }

	auto engine::verify(public_key key, const buffer& hash, const buffer& signature) const -> bool
	{
        const auto& q = static_cast<ecurve::point>(key);
        const auto& p = params_.p;
        const auto& curve = params_.curve;
        const auto& field = curve.field();

        auto h = field.create_element(integer{hash});
        auto s = integer{std::begin(signature), std::begin(signature) + signature.size() / 2};
        auto r = integer{std::begin(signature) + signature.size() / 2, std::end(signature)};

        auto rpoint = s * p + r * q;
        auto y = h * rpoint.x;

        return static_cast<integer>(y) == r;
	}
}