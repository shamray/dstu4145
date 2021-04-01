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
        private_key(integer d);

        operator integer() const { return d_; }

    private:
        integer d_;
    };

    class public_key
    {
    public:
        public_key(const std::vector<std::byte>& value);
        public_key(const domain_params& params, const private_key& priv);

        operator ecurve::point() const { return q_; }

    private:
        ecurve::point q_;
    };
}