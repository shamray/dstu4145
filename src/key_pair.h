#pragma once

#include "integer.h"
#include "ecurve.h"
#include <vector>

namespace dstu4145
{
    struct domain_params;

    class private_key
    {
    public:
        private_key(const std::vector<unsigned char>& value);

        operator integer() const { return d_; }

    private:
        integer d_;
    };

    class public_key
    {
    public:
        public_key(const std::vector<unsigned char>& value);
        public_key(const domain_params& params, const private_key& value);

        operator ecurve::point() const { return q_; }

    private:
        ecurve::point q_;
    };
}