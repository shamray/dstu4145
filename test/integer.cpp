#include <gtest/gtest.h>

#include "test-util.h"
#include "integer.h"

using namespace std::literals;

TEST(integer, int_to_buffer)
{
    auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto rbuf = std::vector<std::byte>{};
    auto expected = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    dstu4145::integer_to_buffer(r, std::back_inserter(rbuf));

    EXPECT_EQ(rbuf, expected);
}

TEST(integer, buffer_to_int)
{
    auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    EXPECT_EQ(r, dstu4145::buffer_to_integer(rbuf));
}