#include "polynomial.h"
#include <numeric>

namespace dstu4145
{
    polynomial::polynomial(int m, int x1, int x2, int x3)
    {
        bit_set(value_, m);
        bit_set(value_, x1);
        bit_set(value_, x2);
        bit_set(value_, x3);
        bit_set(value_, 0);
    }

    polynomial::polynomial(int m, int x)
    {
        bit_set(value_, m);
        bit_set(value_, x);
        bit_set(value_, 0);
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

        for (long i = boost::multiprecision::msb(b.value_); i >= 0; i--)
        {
            result.value_ <<= 1;
            if (boost::multiprecision::bit_test(b.value_, static_cast<unsigned>(i)))
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

        long degree = boost::multiprecision::msb(divisor.value_);

        for (long i = boost::multiprecision::msb(dividend.value_); i>=0; i--)
        {
            remainder.value_ <<= 1;
            if (boost::multiprecision::bit_test(dividend.value_, static_cast<unsigned>(i)))
                boost::multiprecision::bit_set(remainder.value_, 0);

            if (boost::multiprecision::bit_test(remainder.value_, static_cast<unsigned>(degree))) {
                remainder.value_ ^= divisor.value_;
                boost::multiprecision::bit_set(quotient.value_, static_cast<unsigned>(i));
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
