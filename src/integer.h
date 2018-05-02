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
        unsigned chunks = bitcount / 8;
        if(bitcount % 8)
            ++chunks;

        for(auto i = 0; i < 32 - chunks; ++i)
            *out++ = 0;

        export_bits(i, out, 8);

    }

    template <class iterator1, class iterator2>
    auto buffer_to_integer(const iterator1& begin, const iterator2& end)
    {
        auto result = integer{0};
        import_bits(result, begin, end);
        return result;
    }

    template <class container>
    auto buffer_to_integer(const container& c)
    {
        return buffer_to_integer(std::begin(c), std::end(c));
    }
}
