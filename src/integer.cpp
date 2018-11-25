#include "integer.h"

namespace dstu4145
{
    namespace adapter
    {
        integer::integer(long long value)
            : impl_(value)
        {}

        integer::integer(integer::impl_t impl)
            : impl_(std::move(impl))
        {

        }

        auto operator+(const integer &a, const integer &b) -> integer
        {
            return integer{a.impl_ + b.impl_};
        }

        auto operator==(const integer &a, const integer &b) -> bool
        {
            return a.impl_ == b.impl_;
        }

        auto operator!=(const integer &a, const integer &b) -> bool
        {
            return a.impl_ != b.impl_;
        }
    }
}

