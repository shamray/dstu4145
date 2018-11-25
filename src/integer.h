#pragma once

#include <boost/multiprecision/cpp_int.hpp>
#include "is_container.h"

namespace dstu4145
{
    using integer = boost::multiprecision::number<
        boost::multiprecision::cpp_int_backend<
            256,
            4096,
            boost::multiprecision::unsigned_magnitude,
            boost::multiprecision::unchecked,
            void
        >
    >;

    template <class iterator>
    void integer_to_buffer(const integer& i, iterator out);

    template <class iterator1, class iterator2>
    auto buffer_to_integer(const iterator1& begin, const iterator2& end);

    namespace adapter
    {
        class integer
        {
            friend auto operator+ (const integer& a, const integer& b) -> integer;
            friend auto operator* (const integer& a, const integer& b) -> integer;
            friend auto operator==(const integer& a, const integer& b) -> bool;
            friend auto operator!=(const integer& a, const integer& b) -> bool;

        public:
            integer(long long value);

            explicit
            integer(std::string_view hex);

            template <class container, class = std::enable_if_t<is_container<container>::value>> explicit
            integer (const container& c)
                : impl_(buffer_to_integer(std::begin(c), std::end(c)))
            {
            }

            template <class iterator>
            void to_buffer(iterator out) const;

        private:
            using impl_t = boost::multiprecision::number<
                boost::multiprecision::cpp_int_backend<
                    256,
                    4096,
                    boost::multiprecision::unsigned_magnitude,
                    boost::multiprecision::unchecked,
                    void
                >
            >;

            integer(impl_t impl);

        private:
            impl_t impl_;
        };

        template<class iterator>
        void integer::to_buffer(iterator out) const
        {
            return integer_to_buffer(impl_, out);
        }
    }

    template <class iterator>
    void integer_to_buffer(const integer& i, iterator out)
    {
        unsigned bitcount = boost::multiprecision::msb(i) + 1;
        constexpr auto chunk_size = uint8_t{8};
        constexpr bool msv_first = true;
        unsigned chunks = bitcount / chunk_size;
        if(bitcount % 8)
            ++chunks;

        for(auto i = 0u; i < 32 - chunks; ++i)
            *out++ = std::byte{0};

        if(!i)
            return;

        int bit_location = bitcount - chunk_size;
        int bit_step = -static_cast<int>(chunk_size);
        while(bit_location % bit_step) ++bit_location;

        do
        {
            *out = static_cast<std::byte>(boost::multiprecision::detail::extract_bits(i.backend(), bit_location, chunk_size, boost::mpl::false_()));
            ++out;
            bit_location += bit_step;
        } while((bit_location >= 0) && (bit_location < (int)bitcount));
    }

    template <class iterator1, class iterator2>
    auto buffer_to_integer(const iterator1& begin, const iterator2& end)
    {
        auto result = integer{0};
        import_bits(result, begin, end, 8);
        return result;
    }

    template <class container>
    auto buffer_to_integer(const container& c)
    {
        return buffer_to_integer(std::begin(c), std::end(c));
    }
}
