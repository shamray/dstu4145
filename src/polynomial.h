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

