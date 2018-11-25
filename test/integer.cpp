#include <gtest/gtest.h>

#include "test-utils.h"
#include "integer.h"

#include <vector>

using namespace std::literals;

using dstu4145::adapter::integer;

TEST(integer, int_to_buffer)
{
    const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    const auto expected = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    auto rbuf = std::vector<std::byte>{};
    r.to_buffer(std::back_inserter(rbuf));

    EXPECT_EQ(rbuf, expected);
}

TEST(integer, construct_from_buffer)
{
    const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    EXPECT_EQ(r, integer{rbuf});
}

TEST(integer, construct_from_buffer_iterators)
{
    const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    EXPECT_EQ(r, integer(rbuf.begin(), rbuf.end()));
}

TEST(integer, bit_set_first)
{
    auto x = integer{};
    x.bit_set(0);

    EXPECT_EQ(x, 1);
}

TEST(integer, bit_set_middle)
{
    auto x = integer{};
    x.bit_set(3);
    x.bit_set(2);

    EXPECT_EQ(x, 0xC);
}

TEST(integer, addition_small)
{
    const auto a = integer{2};
    const auto b = integer{3};
    EXPECT_EQ(a + b, 5);
}

TEST(integer, equals_to_itself)
{
    const auto x = integer{42};
    EXPECT_EQ(x, x);
}

TEST(integer, not_equals_plus_one)
{
    const auto x = integer{42};
    EXPECT_NE(x, x + 1);
}

TEST(integer, addition_large)
{
    const auto a = integer{"50505050505050505050505050505050"};
    const auto b = integer{"51515151515151515151515151515151"};
    const auto s = integer{"A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1"};
    EXPECT_EQ(a + b, s);
}

TEST(integer, multiplication_by_2_is_two_additions)
{
    const auto a = integer{0x1234567890ABCD13};
    EXPECT_EQ(a + a, 2 * a);
}

TEST(integer, multiplication_by_3_is_three_additions)
{
    const auto a = integer{0x1234567890ABCD13};
    EXPECT_EQ(a + a + a, 3 * a);
}
