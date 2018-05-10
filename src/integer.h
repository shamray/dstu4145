#pragma once

#include <boost/multiprecision/cpp_int.hpp>

namespace dstu4145
{
    using integer = boost::multiprecision::uint256_t;
    using integer2 = boost::multiprecision::uint512_t;

    template <class iterator>
    void integer_to_buffer(const integer& i, iterator out)
    {
        unsigned bitcount = boost::multiprecision::backends::eval_msb_imp(i.backend()) + 1;
        constexpr auto chunk_size = 8;
        constexpr bool msv_first = true;
        unsigned chunks = bitcount / chunk_size;
        if(bitcount % 8)
            ++chunks;

        for(auto i = 0; i < 32 - chunks; ++i)
            *out++ = std::byte{0};

        if(!i)
            return;

        int bit_location = msv_first ? bitcount - chunk_size : 0;
        int bit_step = msv_first ? -static_cast<int>(chunk_size) : chunk_size;
        while(bit_location % bit_step) ++bit_location;

        do
        {
            *out = std::byte{boost::multiprecision::detail::extract_bits(i.backend(), bit_location, chunk_size, integer::backend_type::trivial_tag())};
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
