#pragma once

#include "integer.h"

#include <tuple>
#include <boost/operators.hpp>

namespace dstu4145::in
{
    class polynomial: boost::equality_comparable<polynomial>
    {
        friend auto operator==(const polynomial&, const polynomial&) -> bool;
        friend auto operator+(const polynomial& a, const polynomial& b) -> polynomial;
        friend auto operator*(const polynomial& a, const polynomial& b) -> polynomial;
        friend auto operator/(const polynomial& a, const polynomial& b) -> std::tuple<polynomial, polynomial>;
        friend auto operator%(const polynomial& a, const polynomial& b) -> polynomial;
        friend auto operator<<(std::ostream& os, const polynomial& x) -> std::ostream&;

    public:
        polynomial() = default;
        polynomial(int m, int x1, int x2, int x3);
        polynomial(int m, int x);
        explicit polynomial(integer value);
        explicit polynomial(int value) : polynomial(integer{ value }) {}

        template <class iterator1, class iterator2>
        polynomial(const iterator1& begin, const iterator2& end)
        {
            auto size_in_bits = std::distance(begin, end) * 8;
            for (auto i = begin; i != end; ++i) {
                for (auto j = 1; j <= 8; ++j) {
                    auto bitnum = size_in_bits - 8 * std::distance(begin, i) - j;
                    if (((std::byte{ 1 } << (8 - j)) & *i) != std::byte{ 0 })
                        bit_set(bitnum);
                }
            }
        }

        template <class container, class = std::enable_if_t<is_container<container>::value>> explicit
        polynomial(const container& c)
            : polynomial(std::begin(c), std::end(c))
        {
        }

        explicit polynomial(std::string_view hex);

        explicit operator integer()     { return value_; }

        auto is_zero() const            { return value_ == 0; }
        void bit_set(size_t m)          { value_.bit_set(m); }
        void bit_unset(size_t m)        { value_.bit_unset(m); }
        bool bit_test(size_t m) const   { return value_.bit_test(m); };
        auto msb() const                { return value_.msb(); }

    private:
        integer value_;
    };

}

namespace dstu4145::vec
{
    class polynomial: boost::equality_comparable<polynomial>
    {
        friend auto operator==(const polynomial&, const polynomial&) -> bool;
        friend auto operator+(const polynomial& a, const polynomial& b) -> polynomial;
        friend auto operator*(const polynomial& a, const polynomial& b) -> polynomial;
        friend auto operator/(const polynomial& a, const polynomial& b) -> std::tuple<polynomial, polynomial>;
        friend auto operator%(const polynomial& a, const polynomial& b) -> polynomial;
        friend auto operator<<(std::ostream& os, const polynomial& x) -> std::ostream&;

    public:
        polynomial() = default;
        polynomial(int m, int x1, int x2, int x3);
        polynomial(int m, int x);
        explicit polynomial(const integer& value);
        explicit polynomial(int value) : polynomial(integer{ value }) {}

        template <class iterator1, class iterator2>
        polynomial(const iterator1& begin, const iterator2& end)
        {
            auto size_in_bits = std::distance(begin, end) * 8;
            for (auto i = begin; i != end; ++i) {
                for (auto j = 1; j <= 8; ++j) {
                    auto bitnum = size_in_bits - 8 * std::distance(begin, i) - j;
                    if (((std::byte{1} << (8 - j)) & *i) != std::byte{0})
                        bit_set(bitnum);
                }
            }
        }

        template <class container, class = std::enable_if_t<is_container<container>::value>> explicit
            polynomial(const container& c)
            : polynomial(std::begin(c), std::end(c))
        {
        }

        explicit polynomial(std::string_view hex);


        polynomial(polynomial&&) = default;
        polynomial& operator=(polynomial&&) = default;
        polynomial(const polynomial&) = default;
        polynomial& operator=(const polynomial&) = default;

        explicit operator integer() const;

        auto is_zero() const -> bool;
        void bit_set(size_t m);
        void bit_unset(size_t m);
        bool bit_test(size_t m) const;
        size_t msb() const;

        bool left_shift();

        template<class iterator>
        void to_buffer(iterator out) const
        {
            auto bits = is_zero() ? 0 : (msb() + 1);
            auto size = (bits % 8) ? (bits / 8 + 1) : (bits / 8);
            auto chunk_size = sizeof(decltype(value_)::value_type);
            auto to_copy = (size % chunk_size) ? (size % chunk_size) : chunk_size;

            for (auto v = value_.rbegin(); v != value_.rend(); ++v) {
                auto chunk = *v;
                std::vector<std::byte> chunk_bytes;
                for (auto i = 0; i < to_copy; ++i) {
                    chunk_bytes.push_back(std::byte{ chunk & 0xFF });
                    chunk = chunk >> 8;
                }
                out = std::copy(chunk_bytes.rbegin(), chunk_bytes.rend(), out);
                to_copy = chunk_size;
            }
        }


        static constexpr auto internal_chunk_size() { return 8 * sizeof(decltype(value_)::value_type); }

    private:
        std::vector<unsigned> value_;
    };

}

namespace dstu4145
{
    using polynomial = vec::polynomial;

    auto extended_euqlid(polynomial f,  polynomial c, polynomial mod) -> std::tuple<polynomial, polynomial, polynomial>;
}

