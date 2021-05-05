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

        template<class iterator>
        void to_buffer(iterator out) const { d_.to_buffer(out, d_.size_in_bytes() * 8); }

        friend auto operator==(const private_key&, const private_key&) -> bool;
        friend auto operator!=(const private_key&, const private_key&) -> bool;

    private:
        integer d_;
    };

    inline auto operator==(const private_key& a, const private_key& b) -> bool
    {
        return a.d_ == b.d_;
    }

    inline auto operator!=(const private_key& a, const private_key& b) -> bool
    {
        return a.d_ != b.d_;
    }

    class public_key
    {
    public:
        public_key(const domain_params& params, const std::vector<std::byte>& value);
        public_key(const domain_params& params, const private_key& priv);

        operator ecurve_point() const { return q_; }

        friend auto operator==(const public_key&, const public_key&) -> bool;
        friend auto operator!=(const public_key&, const public_key&) -> bool;

        template<class iterator>
        void to_buffer(iterator out) const 
        { 
            auto p = q_.compress();
            p.to_buffer(out);
        }

    private:
        ecurve_point q_;
    };

    inline auto operator==(const public_key& a, const public_key& b) -> bool
    {
        return a.q_ == b.q_;
    }

    inline auto operator!=(const public_key& a, const public_key& b) -> bool
    {
        return a.q_ != b.q_;
    }

    inline auto operator<< (std::ostream& os, const public_key& pk) -> std::ostream&
    {
        return os << static_cast<ecurve_point>(pk);
    }
}
