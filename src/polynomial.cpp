#include "polynomial.h"

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

    auto extended_euqlid_r(polynomial f,  polynomial c, polynomial mod) -> std::tuple<polynomial, polynomial, polynomial>
    {
        if (c.is_zero())
            return std::make_tuple(f, polynomial{1}, polynomial{0});

        auto [r, q] = f / c;

        auto [d, a, b] = extended_euqlid_r(c, r, mod);
        return std::make_tuple(d, b, a + q * b % mod);

    }
    auto extended_euqlid(polynomial f,  polynomial c, polynomial mod) -> std::tuple<polynomial, polynomial, polynomial>
    {
        auto a = polynomial{1};
        auto b = polynomial{0};
        auto d = f;

        auto cc = c;

        std::vector<std::tuple<polynomial, polynomial, polynomial>> stack;
        while(c != polynomial{0}) {
            auto [r, q] = f / c;
            stack.emplace_back(r, q, c);
            f = c;
            c = std::get<0>(stack.back());
        }

        for (auto i = stack.rbegin(); i != stack.rend(); ++i) {
            auto [r, q, c] = *i;
            auto bb = b;
            auto aa = a;
            d = std::get<2>(stack.back());
            a = b;
            b = aa + q * bb % mod;
        }

        return std::tuple(d, a, b);
    }
}
