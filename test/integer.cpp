#include <gtest/gtest.h>

#include "test-utils.h"
#include "integer.h"

using namespace std::literals;

using dstu4145::adapter::integer;

TEST(integer, int_to_buffer)
{
    const auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    const auto expected = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    auto rbuf = std::vector<std::byte>{};
    dstu4145::integer_to_buffer(r, std::back_inserter(rbuf));

    EXPECT_EQ(rbuf, expected);
}

TEST(integer, buffer_to_int)
{
    const auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    EXPECT_EQ(r, dstu4145::buffer_to_integer(rbuf));
}

TEST(integer, addition_small)
{
    const auto a = integer{2};
    const auto b = integer{3};
    EXPECT_EQ(a + b, 5);
}

TEST(integer, addition_large)
{
    const auto a = dstu4145::integer{"0x50505050505050505050505050505050"};
    const auto b = dstu4145::integer{"0x51515151515151515151515151515151"};
    const auto s = dstu4145::integer{"0xA1A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1"};
    EXPECT_EQ(a + b, s);
}

TEST(integer, multiplication_by_2_is_two_additions)
{
    const auto a = dstu4145::integer{0x1234567890ABCD13};
    EXPECT_EQ(a + a, 2 * a);
}

TEST(integer, multiplication_by_3_is_three_additions)
{
    const auto a = dstu4145::integer{0x1234567890ABCD13};
    EXPECT_EQ(a + a + a, 3 * a);
}
