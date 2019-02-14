#include "polynomial.h"
#include <numeric>

namespace dstu4145::in
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
}

namespace dstu4145::vec
{
    polynomial::polynomial(int m, int x1, int x2, int x3)
    {
        bit_set(m);
        bit_set(x1);
        bit_set(x2);
        bit_set(x3);
        bit_set(0);
    }

    polynomial::polynomial(int m, int x)
    {
        bit_set(m);
        bit_set(x);
        bit_set(0);
    }

    polynomial::polynomial(dstu4145::integer value)
    {
        if (value == 0)
            return;

        for (auto i = 0; i <= value.msb(); ++i) {
            if (value.bit_test(i))
                bit_set(i);
        }
    }

    auto operator==(const polynomial& a, const polynomial& b) -> bool
    {
        for (auto i = 0; i < std::min(a.value_.size(), b.value_.size()); ++i) {
            if (a.value_[i] != b.value_[i])
                return false;
        }

        for (auto i = std::min(a.value_.size(), b.value_.size()); i < std::max(a.value_.size(), b.value_.size()); ++i) {
            if (i < a.value_.size() && a.value_[i] != 0)
                return false;
            if (i < b.value_.size() && b.value_[i] != 0)
                return false;
        }

        return true;
    }

    auto operator+(const polynomial& a, const polynomial& b) -> polynomial
    {
        auto result = polynomial{};
        result.value_.resize(std::max(a.value_.size(), b.value_.size()));

        for (auto i = 0; i < std::min(a.value_.size(), b.value_.size()); ++i) {
            result.value_[i] = a.value_[i] ^ b.value_[i];
        }
        for (auto i = std::min(a.value_.size(), b.value_.size()); i < std::max(a.value_.size(), b.value_.size()); ++i) {
            if (i < a.value_.size() && a.value_[i] != 0)
                result.value_[i] = a.value_[i];
            else if (i < b.value_.size() && b.value_[i] != 0)
                result.value_[i] = b.value_[i];
        }
        return result;
    }

    void left_shift(std::vector<unsigned>& vec)
    {
        bool carry = false;
        for (auto& x: vec) {
            auto nextCarry = ((1 << (sizeof(std::vector<unsigned>::value_type)-1) ) & x) != 0;
            x <<= 1;
            if (carry)
                x &= 1;
            carry = nextCarry;
        }
        if (carry)
            vec.push_back(1);
    }

    auto operator*(const polynomial& a, const polynomial& b) -> polynomial
    {
        auto result = polynomial{};
        if (b == polynomial{})
            return result;

        result.value_.resize(std::max(a.value_.size(), b.value_.size()));

        for (long i = b.value_.size() * sizeof(decltype(a.value_)::value_type); i >= 0; i--)
        {
            left_shift(result.value_);
            if (b.bit_test(static_cast<unsigned>(i)))
                result = result + a;
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

        long degree = divisor.msb();

        for (long i = dividend.msb(); i>=0; i--)
        {
            left_shift(remainder.value_);
            if (dividend.bit_test(static_cast<unsigned>(i)))
                remainder.bit_set(0);

            if (remainder.bit_test(static_cast<unsigned>(degree))) {
                remainder = remainder + divisor;
                quotient.bit_set(static_cast<unsigned>(i));
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
        return os << static_cast<integer>(x);
    }

    void polynomial::bit_set(size_t m)
    {
        auto ix = m / sizeof(decltype(value_)::value_type);
        auto bi = m % sizeof(decltype(value_)::value_type);

        if (value_.size() <= ix)
            value_.resize(ix + 1);

        auto x = (1 << bi);

        value_[ix] |= x;
    }

    void polynomial::bit_unset(size_t m)
    {
        auto ix = m / sizeof(decltype(value_)::value_type);
        auto bi = m % sizeof(decltype(value_)::value_type);

        if (value_.size() < ix)
            value_.resize(ix + 1);

        value_[ix] &= !(1 << (bi - 1));
    }

    auto polynomial::is_zero() const -> bool
    {
        for (auto x: value_) {
            if (x != 0)
                return false;
        }
        return true;
    }

    bool polynomial::bit_test(size_t m) const
    {
        auto ix = m / sizeof(decltype(value_)::value_type);
        auto bi = m % sizeof(decltype(value_)::value_type);

        if (value_.size() <= ix)
            return false;

        return (value_[ix] & (1 << bi)) != 0;
    }

    size_t polynomial::msb() const
    {
        for (long i = value_.size() - 1; i >= 0; ++i) {
            if (value_[i] == 0)
                continue;

            auto bi = [this, i](){
                auto x = value_[i];
                int msb = 0;
                while (x != 0) {
                    x = x / 2;
                    msb++;
                }

                return msb - 1;
            }();

            return i * sizeof(decltype(value_)::value_type) + bi;
        }
    }

    polynomial::operator integer() const
    {
        auto result = integer();
        for (auto i = 0; !value_.empty() && i <= msb(); ++i) {
            if (bit_test(i))
                result.bit_set(i);
        }
        return result;
    }
}

namespace dstu4145
{
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