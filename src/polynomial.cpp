#include "polynomial.h"
#include <numeric>

namespace dstu4145
{
    polynomial::polynomial(int m, int x1, int x2, int x3)
    {
        value_.bit_set(m);
        value_.bit_set(x1);
        value_.bit_set(x2);
        value_.bit_set(x3);
        value_.bit_set(0);
    }

    polynomial::polynomial(int m, int x)
    {
        value_.bit_set(m);
        value_.bit_set(x);
        value_.bit_set(0);
    }

    polynomial::polynomial(dstu4145::integer value)
        : value_(std::move(value))
    {}

    auto operator==(const polynomial& a, const polynomial& b) -> bool
    {
        return a.value_ == b.value_;
    }

    auto operator+(const polynomial& a, const polynomial& b) -> polynomial
    {
        return polynomial{a.value_ ^ b.value_};
    }

    auto operator*(const polynomial& a, const polynomial& b) -> polynomial
    {
        auto result = polynomial{};

        if (b == polynomial{})
            return result;

        for (long i = b.value_.msb(); i >= 0; i--)
        {
            result.value_ <<= 1;
            if (b.value_.bit_test(static_cast<unsigned>(i)))
                result.value_ ^= a.value_;
        }

        return result;
    }

    auto operator/(const polynomial& dividend, const polynomial& divisor) -> std::tuple<polynomial, polynomial>
    {
        polynomial remainder, quotient;

        if (divisor == polynomial{})
            throw std::logic_error("divide by zero");

        if (dividend == polynomial{})
            return std::make_tuple(remainder, quotient);

        long degree = divisor.value_.msb();

        for (long i = dividend.value_.msb(); i>=0; i--)
        {
            remainder.value_ <<= 1;
            if (dividend.value_.bit_test(static_cast<unsigned>(i)))
                remainder.value_.bit_set(0);

            if (remainder.value_.bit_test(static_cast<unsigned>(degree))) {
                remainder.value_ ^= divisor.value_;
                quotient.value_.bit_set(static_cast<unsigned>(i));
            }
        }

        return std::make_tuple(remainder, quotient);
    }

    auto operator%(const polynomial& a, const polynomial& b) -> polynomial
    {
        auto [r, q] = a / b;
        std::ignore = q;
        return r;
    }

    std::ostream& operator<<(std::ostream& os, const polynomial& x)
    {
        return os << std::hex << x.value_;
    }

    auto compute_remainders(polynomial& x,  polynomial y) 
    {
        std::vector<polynomial> remainders;
        while(y != polynomial{0}) {
            auto [r, q] = x / y;
            x = y;
            y = r;
            remainders.emplace_back(q);
        }
        return remainders;
    }

    auto extended_euqlid(polynomial x,  polynomial y, polynomial mod) -> std::tuple<polynomial, polynomial, polynomial>
    {
        auto remainders = compute_remainders(x, y);
        auto [a, b] = std::accumulate(
            remainders.rbegin(), remainders.rend(),
            std::tuple{polynomial{1}, polynomial{0}},
            [&mod](auto prev, auto q) {
                auto& [a, b] = prev;
                return std::tuple{b, a + q * b % mod};
            }
        );

        return std::tuple(x, a, b);
    }
}
