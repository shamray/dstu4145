#pragma once

#include "integer.h"
#include "ecurve.h"
#include "rng_interface.h"

#include <vector>
#include <tuple>

namespace dstu4145
{
    struct domain_params;

    class private_key
    {
    public:
        private_key(const std::vector<std::byte>& value);

        operator integer() const { return d_; }

    private:
        integer d_;
    };

    class public_key
    {
    public:
        public_key(const std::vector<std::byte>& value);
        public_key(const domain_params& params, const private_key& value);

        operator ecurve::point() const { return q_; }

    private:
        ecurve::point q_;
    };

    auto generate_private_key(const domain_params& params, rng_t random) -> private_key;
	auto generate_key_pair(const domain_params& params, rng_t random) -> std::tuple<private_key, public_key>;
}