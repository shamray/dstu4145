#include "integer.h"

using namespace std::string_literals;

namespace dstu4145::bmp
{
    integer::integer(long long value)
        : impl_(value)
    {
    }

    integer::integer(std::string_view hex)
        : impl_("0x"s.append(hex))
    {
    }

    integer::integer(integer::impl_t impl)
        : impl_(std::move(impl))
    {
    }

    void integer::bit_set(size_t n)
    {
        boost::multiprecision::bit_set(impl_, n);
    }

    void integer::bit_unset(size_t n)
    {
        boost::multiprecision::bit_unset(impl_, n);
    }

    auto integer::bit_test(size_t n) const -> bool
    {
        return boost::multiprecision::bit_test(impl_, n);
    }

    auto integer::msb() const -> size_t
    {
        return boost::multiprecision::msb(impl_);
    }

    auto integer::lsb() const -> size_t
    {
        return boost::multiprecision::lsb(impl_);
    }

    auto operator+(const integer &a, const integer &b) -> integer
    {
        return integer{a.impl_ + b.impl_};
    }

    auto operator*(const integer &a, const integer &b) -> integer
    {
        return integer{a.impl_ * b.impl_};
    }

    auto operator/(const integer &a, const integer &b) -> integer
    {
        return integer{a.impl_ / b.impl_};
    }

    auto operator%(const integer &a, const integer &b) -> integer
    {
        return integer{a.impl_ % b.impl_};
    }

    auto operator^(const integer &a, const integer &b) -> integer
    {
        return integer{a.impl_ ^ b.impl_};
    }

    auto operator<<(const integer &a, size_t bits) -> integer
    {
        return a.impl_ << bits;
    }

    auto operator==(const integer &a, const integer &b) -> bool
    {
        return a.impl_ == b.impl_;
    }

    auto operator!=(const integer &a, const integer &b) -> bool
    {
        return a.impl_ != b.impl_;
    }

    auto operator<(const integer &a, const integer &b) -> bool
    {
        return a.impl_ < b.impl_;
    }

    auto operator>(const integer &a, const integer &b) -> bool
    {
        return a.impl_ > b.impl_;
    }

    auto operator<=(const integer &a, const integer &b) -> bool
    {
        return a.impl_ <= b.impl_;
    }

    auto operator>=(const integer &a, const integer &b) -> bool
    {
        return a.impl_ >= b.impl_;
    }

    auto operator<<(std::ostream& os, const integer& a) -> std::ostream&
    {
        return os << std::hex << a.impl_;
    }
}

namespace dstu4145::ossl
{
    namespace
    {
        template <class T>
        class mem_guard
        {
        public:
            mem_guard(const T* ptr) : ptr_(ptr) {}
            ~mem_guard() { OPENSSL_free(const_cast<T*>(ptr_)); }

            auto get()  { return ptr_; }

        private:
            const T* ptr_;
        };
    }

    integer::~integer()
    {
        BN_free(impl_);
        BN_CTX_free(ctx_);
    }

    integer::integer()
        : impl_(BN_new())
        , ctx_(BN_CTX_new())
    {

    }

    integer::integer(long long value)
        : impl_(BN_new())
        , ctx_(BN_CTX_new())
    {
        BN_set_word(impl_, value);
    }

    integer::integer(const std::string& hex)
        : impl_(BN_new())
        , ctx_(BN_CTX_new())
    {
        BN_hex2bn(&impl_, hex.c_str());
    }

    auto operator<<(std::ostream &os, const integer &a) -> std::ostream&
    {
        auto s = mem_guard{ BN_bn2hex(a.impl_) };
        return os << s.get();
    }

    auto operator==(const integer &a, const integer &b) -> bool
    {
        auto x = BN_cmp(a.impl_, b.impl_);
        return x == 0;
    }

    void integer::bit_set(size_t n)
    {
        auto r = BN_set_bit(impl_, n);
        if (r == 0)
            throw std::runtime_error("error");
    }

    auto operator!=(const integer &a, const integer &b) -> bool
    {
        return BN_cmp(a.impl_, b.impl_) != 0;
    }

    void integer::bit_unset(size_t n)
    {
        auto r = BN_clear_bit(impl_, n);
        if (r == 0)
            throw std::runtime_error("error");
    }

    auto integer::bit_test(size_t n) const -> bool
    {
        return BN_is_bit_set(impl_, n) != 0;
    }

    auto integer::msb() const -> size_t
    {
        auto r = BN_num_bytes(impl_) * 8;

        for (auto i = signed { r }; i > 0; --i) {
            if (bit_test(i))
                return i;
        }

        throw std::runtime_error("No bits were set in the operand");
    }

    auto integer::lsb() const -> size_t
    {
        for (auto i = 0; i <  BN_num_bytes(impl_) * 8; ++i) {
            if (bit_test(i))
                return i;
        }

        throw std::runtime_error("No bits were set in the operand");
    }

    auto operator+(const integer &a, const integer &b) -> integer
    {
        auto result = integer{};

        auto r = BN_add(result.impl_, a.impl_, b.impl_);
        if (r == 0)
            throw std::runtime_error("error");

        return result;
    }

    auto integer::operator+=(const integer &x) -> integer&
    {
        auto r = BN_add(impl_, impl_, x.impl_);
        if (r == 0)
            throw std::runtime_error("error");

        return *this;
    }

    integer::integer(const integer& x)
        : impl_(BN_dup(x.impl_))
        , ctx_(BN_CTX_new())
    {
        if (impl_ == nullptr)
            throw std::runtime_error("error");
    }

    integer& integer::operator=(const integer& x)
    {
        auto r = BN_copy(impl_, x.impl_);
        if (r == nullptr)
            throw std::runtime_error("error");

        return *this;
    }

    integer::integer(integer&& x)
        : impl_(x.impl_)
        , ctx_(BN_CTX_new())
    {
        x.impl_ = nullptr;
    }

    integer &integer::operator=(integer&& x)
    {
        impl_ = x.impl_;
        x.impl_ = nullptr;

        return *this;
    }

    auto operator^(const integer &a, const integer &b) -> integer
    {
        auto result = integer{};

        bn_check_top(a.impl_);
        bn_check_top(b.impl_);

        const auto [at, bt] = [&a,&b]() {
            if (a.impl_->top < b.impl_->top)
                return std::tuple{b.impl_, a.impl_};
            else
                return std::tuple{a.impl_, b.impl_};
        }();

        if (bn_wexpand(result.impl_, at->top) == nullptr)
            throw std::runtime_error("error");

        for (auto i = 0; i < bt->top; ++i) {
            result.impl_->d[i] = at->d[i] ^ bt->d[i];
        }
        for (auto i = bt->top; i < at->top; ++i) {
            result.impl_->d[i] = at->d[i];
        }

        result.impl_->top = at->top;
        bn_correct_top(result.impl_);

        return result;
    }

    auto integer::operator^=(const integer &x) -> integer&
    {
        return *this = *this ^ x;
    }

    auto operator<=(const integer &a, const integer &b) -> bool
    {
        auto x = BN_cmp(a.impl_, b.impl_);
        return x <= 0;
    }

    auto operator>=(const integer &a, const integer &b) -> bool
    {
        auto x = BN_cmp(a.impl_, b.impl_);
        return x >= 0;
    }

    auto operator>(const integer &a, const integer &b) -> bool
    {
        auto x = BN_cmp(a.impl_, b.impl_);
        return x > 0;

    }

    auto operator<(const integer &a, const integer &b) -> bool
    {
        auto x = BN_cmp(a.impl_, b.impl_);
        return x < 0;

    }

    auto operator*(const integer &a, const integer &b) -> integer
    {
        auto result = integer{};

        auto r = BN_mul(result.impl_, a.impl_, b.impl_, a.ctx_);
        if (r == 0)
            throw std::runtime_error("error");

        return result;
    }

    auto integer::operator*=(const integer &x) -> integer &
    {
        auto r = BN_mul(impl_, impl_, x.impl_, ctx_);
        if (r == 0)
            throw std::runtime_error("error");

        return *this;
    }

    auto divide(const integer &a, const integer &b) -> std::pair<integer, integer>
    {
        auto dv = integer{};
        auto rm = integer{};

        auto r = BN_div(dv.impl_, rm.impl_, a.impl_, b.impl_, a.ctx_);
        if (r == 0)
            throw std::runtime_error("error");

        return std::pair(dv, rm);
    }

    auto operator/(const integer &a, const integer &b) -> integer
    {
        auto [dv, rm] =  divide(a, b);
        return dv;
    }

    auto operator%(const integer &a, const integer &b) -> integer
    {
        auto [dv, rm] =  divide(a, b);
        return rm;
    }

    auto operator<<(const integer &a, int bits) -> integer
    {
        auto result = integer{};

        auto r = BN_lshift(result.impl_, a.impl_, bits);
        if (r == 0)
            throw std::runtime_error("error");

        return result;
    }
}


