#include "engine.h"
#include "rng.h"

#include <cassert>
#include <iterator>
#include <stdexcept>

namespace dstu4145
{
    namespace
    {
        // Clear every bit at index `bits` and above.
        auto truncate(integer value, size_t bits) -> integer
        {
            for (auto i = bits; i < value.size_in_bytes() * 8; ++i)
                value.bit_unset(i);

            return value;
        }

        // DSTU 4145: the hash is interpreted as a field element; if it reduces to
        // zero, 1 is used instead.
        auto hash_element(const gf2m& field, const buffer& hash) -> gf2m_element
        {
            auto h = field.element(polynomial{hash});
            return h.is_zero() ? field.element(1) : h;
        }
    }

    engine::engine(domain_params params)
        : params_{ std::move(params) }
    {
    }

    auto engine::generate_private_key(rng_t random) const -> private_key
    {
        auto n = params_.n;
        auto d = gen_random_integer(random, n.msb());

        return d;
    }

    auto engine::generate_key_pair(rng_t random) const -> std::tuple<private_key, public_key>
    {
        auto pri = generate_private_key(random);
        auto pub = public_key{params_, pri};

        return { pri, pub };
    }

    auto engine::compute_presignature(rng_t random) const -> presignature
    {
        const auto& p = params_.p;
        const auto& n = params_.n;

        for (;;) {
            auto e = gen_random_integer(random, n.msb());
            assert(e < n);

            auto fe = (e * p).x;
            if (!fe.is_zero())
                return {e, fe};
        }
    }

    // DSTU 4145: r is the field element y truncated to msb(n) bits, so both
    // components of the signature are guaranteed to be less than n.
    auto engine::try_sign(const presignature& ps, const private_key& key, const buffer& hash) const
        -> std::optional<buffer>
    {
        const auto& curve = params_.curve;
        const auto& n = params_.n;
        const auto& d = static_cast<integer>(key);

        const auto& [e, fe] = ps;

        auto y = hash_element(curve.field(), hash) * fe;
        auto r = truncate(static_cast<integer>(y), n.msb());
        if (r == integer{ 0 })
            return std::nullopt;

        auto s = (e + (d * r) % n) % n;
        if (s == integer{ 0 })
            return std::nullopt;

        const auto half_size = n.size_in_bytes() * 8;

        auto result = buffer{};
        s.to_buffer(std::back_inserter(result), half_size);
        r.to_buffer(std::back_inserter(result), half_size);

        return result;
    }

    auto engine::sign(presignature ps, private_key key, const buffer& hash) const -> buffer
    {
        auto result = try_sign(ps, key, hash);
        if (!result)
            throw std::runtime_error("degenerate presignature, sign again with a fresh nonce");

        return *std::move(result);
    }

    auto engine::sign(rng_t random, private_key key, const buffer& hash) const -> buffer
    {
        for (;;) {
            if (auto result = try_sign(compute_presignature(random), key, hash))
                return *std::move(result);
        }
    }

    auto engine::verify(public_key key, const buffer& hash, const buffer& signature) const -> bool
    {
        const auto& curve = params_.curve;
        const auto& n = params_.n;

        if (signature.empty() || signature.size() % 2 != 0)
            return false;

        const auto middle = std::begin(signature) + signature.size() / 2;
        auto s = integer{std::begin(signature), middle};
        auto r = integer{middle, std::end(signature)};

        // Rejecting zero keeps multiply() away from lsb(), which throws on it;
        // rejecting r, s >= n rules out the s + n variant of a valid signature.
        if (r == integer{ 0 } || r >= n || s == integer{ 0 } || s >= n)
            return false;

        const auto& q = static_cast<ecurve_point>(key);
        const auto& p = params_.p;

        auto rpoint = s * p + r * q;
        auto y = hash_element(curve.field(), hash) * rpoint.x;

        return truncate(static_cast<integer>(y), n.msb()) == r;
    }
}
