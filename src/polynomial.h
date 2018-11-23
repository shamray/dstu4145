#pragma once

#include "integer.h"

#include <tuple>

namespace dstu4145
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

        explicit operator integer() { return value_; }

        auto is_zero() const { return value_ == 0; }

    private:
        integer value_;
    };

    auto extended_euqlid(polynomial f,  polynomial c, polynomial mod) -> std::tuple<polynomial, polynomial, polynomial>;
}

