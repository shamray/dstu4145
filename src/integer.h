#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include <openssl/bn.h>
#include "is_container.h"

namespace dstu4145::bmp
{
    class integer
    {
        friend auto operator+ (const integer& a, const integer& b) -> integer;
        friend auto operator* (const integer& a, const integer& b) -> integer;
        friend auto operator/ (const integer& a, const integer& b) -> integer;
        friend auto operator% (const integer& a, const integer& b) -> integer;
        friend auto operator^ (const integer& a, const integer& b) -> integer;
        friend auto operator<<(const integer& a, size_t bits) -> integer;
        friend auto operator< (const integer& a, const integer& b) -> bool;
        friend auto operator> (const integer& a, const integer& b) -> bool;
        friend auto operator<=(const integer& a, const integer& b) -> bool;
        friend auto operator>=(const integer& a, const integer& b) -> bool;
        friend auto operator==(const integer& a, const integer& b) -> bool;
        friend auto operator!=(const integer& a, const integer& b) -> bool;
        friend auto operator<<(std::ostream& os, const integer& a) -> std::ostream&;

    public:
        integer() = default;

        integer(long long value);

        explicit
        integer(std::string_view hex);

        template <class iterator1, class iterator2>
        integer(const iterator1& begin, const iterator2& end)
            : impl_{0}
        {
            import_bits(impl_, begin, end, 8);
        }

        template <class container, class = std::enable_if_t<is_container<container>::value>> explicit
        integer(const container& c)
            : integer(std::begin(c), std::end(c))
        {
        }

        template <class iterator>
        void to_buffer(iterator out, size_t size = 256) const;

        void bit_set(size_t n);
        void bit_unset(size_t n);
        auto bit_test(size_t n) const -> bool;
        auto msb() const -> size_t;
        auto lsb() const -> size_t;

        auto& operator+= (const integer& x)  { impl_ += x.impl_; return *this; }
        auto& operator*= (const integer& x)  { impl_ *= x.impl_; return *this; }
        auto& operator^= (const integer& x)  { impl_ ^= x.impl_; return *this; }
        auto& operator<<=(size_t x)          { impl_ <<= x;      return *this; }

    private:
        using impl_t = boost::multiprecision::number<
            boost::multiprecision::cpp_int_backend<
                256,
                4096,
                boost::multiprecision::unsigned_magnitude,
                boost::multiprecision::unchecked,
                void
            >
        >;

        integer(impl_t impl);

    private:
        impl_t impl_;
    };

    template<class iterator>
    void integer::to_buffer(iterator out, size_t size) const
    {
        unsigned bitcount = boost::multiprecision::msb(impl_) + 1;
        constexpr auto chunk_size = uint8_t{8};
        constexpr bool msv_first = true;
        unsigned chunks = bitcount / chunk_size;
        if(bitcount % chunk_size)
            ++chunks;

        for(auto i = 0u; i < size / chunk_size - chunks; ++i)
            *out++ = std::byte{0};

        if(!impl_)
            return;

        int bit_location = bitcount - chunk_size;
        int bit_step = -static_cast<int>(chunk_size);
        while(bit_location % bit_step) ++bit_location;

        do
        {
            *out = static_cast<std::byte>(boost::multiprecision::detail::extract_bits(impl_.backend(), bit_location, chunk_size, boost::mpl::false_()));
            ++out;
            bit_location += bit_step;
        } while((bit_location >= 0) && (bit_location < (int)bitcount));
    }
}

namespace dstu4145::ossl
{
    class integer
    {
        friend auto operator+ (const integer& a, const integer& b) -> integer;
        friend auto operator* (const integer& a, const integer& b) -> integer;
        friend auto operator/ (const integer& a, const integer& b) -> integer;
        friend auto operator% (const integer& a, const integer& b) -> integer;
        friend auto operator^ (const integer& a, const integer& b) -> integer;
        friend auto operator<<(const integer& a, size_t bits) -> integer;
        friend auto operator< (const integer& a, const integer& b) -> bool;
        friend auto operator> (const integer& a, const integer& b) -> bool;
        friend auto operator<=(const integer& a, const integer& b) -> bool;
        friend auto operator>=(const integer& a, const integer& b) -> bool;
        friend auto operator==(const integer& a, const integer& b) -> bool;
        friend auto operator!=(const integer& a, const integer& b) -> bool;
        friend auto operator<<(std::ostream& os, const integer& a) -> std::ostream&;

    public:
        ~integer();
        integer();

        integer(long long value);

        explicit
        integer(const std::string& hex);

        template <class iterator1, class iterator2>
        integer(const iterator1& begin, const iterator2& end)
            : impl_(BN_new())
        {
            auto temp = std::vector<std::byte>{};
            std::copy(begin, end, std::back_inserter(temp));
            auto result = BN_bin2bn(reinterpret_cast<unsigned char*>(temp.data()), temp.size(), impl_);
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

        void bit_set(size_t n);
        void bit_unset(size_t n);
        auto bit_test(size_t n) const -> bool;
        auto msb() const -> size_t;
        auto lsb() const -> size_t;

        template <class iterator>
        void to_buffer(iterator out, size_t size = 256) const;

    private:
        BIGNUM* impl_;
    };

    template<class iterator>
    void integer::to_buffer(iterator out, size_t size) const
    {
        for(auto i = 0u; i < size / 8 - BN_num_bytes(impl_); ++i)
            *out++ = std::byte{0};

        auto result = std::vector<std::byte>(BN_num_bytes(impl_));
        BN_bn2bin(impl_, reinterpret_cast<unsigned char*>(result.data()));
        std::copy(std::begin(result), std::end(result), out);
    }
}

namespace dstu4145
{
    using bmp::integer;
}