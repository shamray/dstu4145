#include "polynomial.h"
#include <numeric>
#include <unordered_map>

using namespace std::string_literals;

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

    polynomial::polynomial(std::string_view hex)
        : value_("0x"s.append(hex))
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

        for (auto i = static_cast<long>(b.value_.msb()); i >= 0; i--)
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

        auto degree = static_cast<long>(divisor.value_.msb());

        for (auto i = static_cast<long>(dividend.value_.msb()); i>=0; i--)
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

    polynomial::polynomial(const dstu4145::integer& value)
    {
        if (value == 0)
            return;

        for (auto i = 0; i <= value.msb(); ++i) {
            if (value.bit_test(i))
                bit_set(i);
        }
    }

    namespace
    {
        auto decode_char(char c) -> std::byte
        {
            static const auto chars = std::unordered_map<char, unsigned char>{
                {'0', 0},   {'1', 1},   {'2', 2},   {'3', 3},
                {'4', 4},   {'5', 5},   {'6', 6},   {'7', 7},
                {'8', 8},   {'9', 9},   {'A', 10},  {'B', 11},
                {'C', 12},  {'D', 13},  {'E', 14},  {'F', 15},
                {'a', 10},  {'b', 11},  {'c', 12},  {'d', 13},  
                {'e', 14},  {'f', 15}
            };

            auto found = chars.find(c);
            if (found == std::end(chars))
                throw std::runtime_error("wrong format");

            return std::byte{ found->second };
        }
    }

    polynomial::polynomial(std::string_view hex)
    {
        assert(std::size(hex) % 2 == 0);

        auto size_in_bits = std::distance(std::begin(hex), std::end(hex)) * 8 / 2;

        for (size_t i = 0; i < std::size(hex); i += 2) {

            auto c1 = decode_char(hex[i]);
            auto c2 = decode_char(hex[i + 1]);

            auto c = std::byte{ (c1 << 4) | c2 };

            for (auto j = 1; j <= 8; ++j) {
                auto bitnum = size_in_bits - 8 * (i / 2) - j;
                if (((std::byte{ 1 } << (8 - j)) & c) != std::byte{ 0 })
                    bit_set(bitnum);
            }
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

    bool polynomial::left_shift()
    {
        bool carry = false;
        for (auto& x: value_) {
            auto nextCarry = ((1 << (internal_chunk_size() - 1) ) & x) != 0;
            x <<= 1;
            if (carry)
                x |= 1;
            carry = nextCarry;
        }
        if (carry)
            value_.push_back(1);

        return carry;
    }

    auto operator*(const polynomial& a, const polynomial& b) -> polynomial
    {
        auto result = polynomial{};
        if (b == polynomial{})
            return result;

        for (auto i = static_cast<long>(b.msb()); i >= 0; i--)
        {
            result.left_shift();
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

        auto degree = static_cast<long>(divisor.msb());

        for (auto i = static_cast<long>(dividend.msb()); i >= 0; i--)
        {
            remainder.left_shift();
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
        auto ix = m / internal_chunk_size();
        auto bi = m % internal_chunk_size();

        if (value_.size() <= ix)
            value_.resize(ix + 1);

        auto x = (1 << bi);

        value_[ix] |= x;
    }

    void polynomial::bit_unset(size_t m)
    {
        auto ix = m / internal_chunk_size();
        auto bi = m % internal_chunk_size();

        if (value_.size() < ix)
            value_.resize(ix + 1);

        value_[ix] &= ~(1 << bi);

        for (; !value_.empty() && value_.back() == 0; value_.pop_back()) {}
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
        auto ix = m / internal_chunk_size();
        auto bi = m % internal_chunk_size();

        if (value_.size() <= ix)
            return false;

        return (value_[ix] & (1 << bi)) != 0;
    }

    size_t polynomial::msb() const
    {
        for (auto i = static_cast<long>(value_.size() - 1); i >= 0; --i) {
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

            return i * internal_chunk_size() + bi;
        }

        throw std::runtime_error("No bits were set in the operand");
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

        return std::tuple{ x, a, b };
    }
}