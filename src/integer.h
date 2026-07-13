#pragma once

#include <openssl/bn.h>
#include "is_container.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iosfwd>
#include <iterator>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace dstu4145::ossl
{
    class integer
    {
        friend auto operator+ (const integer& a, const integer& b) -> integer;
        friend auto operator* (const integer& a, const integer& b) -> integer;
        friend auto operator/ (const integer& a, const integer& b) -> integer;
        friend auto operator% (const integer& a, const integer& b) -> integer;
        friend auto operator^ (const integer& a, const integer& b) -> integer;
        friend auto operator<<(const integer& a, int bits) -> integer;
        friend auto operator< (const integer& a, const integer& b) -> bool;
        friend auto operator> (const integer& a, const integer& b) -> bool;
        friend auto operator<=(const integer& a, const integer& b) -> bool;
        friend auto operator>=(const integer& a, const integer& b) -> bool;
        friend auto operator==(const integer& a, const integer& b) -> bool;
        friend auto operator!=(const integer& a, const integer& b) -> bool;
        friend auto operator<<(std::ostream& os, const integer& a) -> std::ostream&;
        friend auto divide(const integer &a, const integer &b) -> std::pair<integer, integer>;

    public:
        ~integer();
        integer();

        integer(long long value);

        explicit
        integer(std::string_view hex);

        template <class iterator1, class iterator2>
        integer(const iterator1& begin, const iterator2& end)
            : impl_(BN_new())
            , ctx_(BN_CTX_new())
        {
            auto temp = std::vector<std::byte>{};
            std::copy(begin, end, std::back_inserter(temp));
            auto result = BN_bin2bn(reinterpret_cast<unsigned char*>(temp.data()), static_cast<int>(temp.size()), impl_);
        }

        template <class container, class = std::enable_if_t<is_container<container>::value>> explicit
        integer(const container& c)
            : integer(std::begin(c), std::end(c))
        {
        }

        integer(integer&&);
        integer& operator=(integer&&);
        integer(const integer&);
        integer& operator=(const integer&);

        auto operator+= (const integer& x) -> integer&;
        auto operator^= (const integer& x) -> integer&;
        auto operator*= (const integer& x) -> integer&;
        auto operator<<=(int bits) -> integer&;

        void bit_set(size_t n);
        void bit_unset(size_t n);
        auto bit_test(size_t n) const -> bool;
        auto msb() const -> size_t;
        auto lsb() const -> size_t;

        template <class iterator>
        void to_buffer(iterator out, size_t size = 256) const;

        auto size_in_bytes() const -> size_t;

    private:
        BIGNUM* impl_;
        BN_CTX* ctx_;
    };

    template<class iterator>
    void integer::to_buffer(iterator out, size_t size) const
    {
        assert(size / 8 >= BN_num_bytes(impl_));

        for(auto i = 0u; i < size / 8 - BN_num_bytes(impl_); ++i)
            *out++ = std::byte{0};

        auto result = std::vector<std::byte>(BN_num_bytes(impl_));
        BN_bn2bin(impl_, reinterpret_cast<unsigned char*>(result.data()));
        std::copy(std::begin(result), std::end(result), out);
    }
}

namespace dstu4145
{
    using ossl::integer;
}