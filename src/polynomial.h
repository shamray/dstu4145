#pragma once

#include "integer.h"

#include <tuple>

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
        explicit polynomial(integer value);

        explicit operator integer() const;

        auto is_zero() const -> bool;
        void bit_set(size_t m);
        void bit_unset(size_t m);
        bool bit_test(size_t m) const;
        size_t msb() const;

    private:
        std::vector<unsigned> value_;
    };

}

namespace dstu4145
{
    using polynomial = vec::polynomial;

    auto extended_euqlid(polynomial f,  polynomial c, polynomial mod) -> std::tuple<polynomial, polynomial, polynomial>;
}

