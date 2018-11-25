#include "integer.h"

using namespace std::string_literals;

namespace dstu4145
{
    namespace adapter
    {
        integer::integer(long long value)
            : impl_(value)
        {
        }

        integer::integer(std::string_view hex)
            : impl_("0x"s.append(hex))
        {
        }

        integer::integer(integer::impl_t impl)
            : impl_(std::move(impl))
        {
        }

        void integer::bit_set(size_t n)
        {
            boost::multiprecision::bit_set(impl_, n);
        }

        auto integer::bit_test(size_t n) const -> bool
        {
            return boost::multiprecision::bit_test(impl_, n);
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

        auto operator*(const integer &a, const integer &b) -> integer
        {
            return integer{a.impl_ * b.impl_};
        }
    }
}

