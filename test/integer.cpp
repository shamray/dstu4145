#include <gtest/gtest.h>

#include "test-utils.h"
#include "integer.h"

#include <vector>
#include <iterator>

using namespace std::literals;

namespace test_ossl
{
    using dstu4145::ossl::integer;

    TEST(integer_ossl, default_constructor)
    {
        EXPECT_EQ(integer{}, 0);
    }

    TEST(integer_ossl, from_long_and_default)
    {
        EXPECT_NE(integer{}, 1);
    }

    TEST(integer_ossl, from_long)
    {
        EXPECT_NE(integer{42}, 1);
        EXPECT_EQ(integer{42}, 42);
    }

    TEST(integer_ossl, int_to_buffer)
    {
        const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
        const auto expected = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

        auto rbuf = std::vector<std::byte>{};
        r.to_buffer(std::back_inserter(rbuf));

        EXPECT_EQ(rbuf, expected);
    }

    TEST(integer_ossl, construct_from_buffer)
    {
        const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
        const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

        EXPECT_EQ(r, integer{rbuf});
    }

    TEST(integer_ossl, construct_from_buffer_iterators)
    {
        const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
        const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

        EXPECT_EQ(r, integer(rbuf.begin(), rbuf.end()));
    }

    TEST(integer_ossl, bit_set_first)
    {
        auto x = integer{};
        x.bit_set(0);

        EXPECT_EQ(x, 1);
    }

    TEST(integer_ossl, bit_set_middle)
    {
        auto x = integer{};
        x.bit_set(3);
        x.bit_set(2);

        EXPECT_EQ(x, 0xC);
    }

    TEST(integer_ossl, bit_unset_first)
    {
        auto x = integer{1};
        x.bit_unset(0);

        EXPECT_EQ(x, 0);
    }

    TEST(integer_ossl, bit_unset_beyond_msb)
    {
        auto x = integer{1};
        x.bit_unset(1024);

        EXPECT_EQ(x, 1);
    }

    TEST(integer_ossl, bit_set_beyond_msb)
    {
        auto x = integer{1};
        x.bit_set(x.msb() + 1);

        EXPECT_EQ(x, 3);
    }

    TEST(integer_ossl, bit_unset_middle)
    {
        auto x = integer{0xF};
        x.bit_unset(3);
        x.bit_unset(2);

        EXPECT_EQ(x, 3);
    }

    TEST(integer_ossl, bit_unset_zero)
    {
        auto x = integer{0};
        x.bit_unset(0);

        EXPECT_EQ(x, 0);
    }

    TEST(integer_ossl, bit_test)
    {
        auto x = integer{5};
        EXPECT_TRUE (x.bit_test(0));
        EXPECT_FALSE(x.bit_test(1));
        EXPECT_TRUE (x.bit_test(2));
    }

    TEST(integer_ossl, msb_lsb_5)
    {
        auto x = integer{5};
        EXPECT_EQ(x.msb(), 2);
        EXPECT_EQ(x.lsb(), 0);
    }

    TEST(integer_ossl, msb_lsb_1)
    {
        auto x = integer{1};
        EXPECT_EQ(x.msb(), 0);
        EXPECT_EQ(x.lsb(), 0);
    }

    TEST(integer_ossl, msb_lsb_0)
    {
        auto x = integer{0};
        EXPECT_THROW(x.msb(), std::exception);
        EXPECT_THROW(x.lsb(), std::exception);
    }

    TEST(integer_ossl, msb_lsb_2)
    {
        auto x = integer{2};
        EXPECT_EQ(x.msb(), 1);
        EXPECT_EQ(x.lsb(), 1);
    }

    TEST(integer_ossl, addition_small)
    {
        const auto a = integer{2};
        const auto b = integer{3};
        EXPECT_EQ(a + b, 5);
    }

    TEST(integer_ossl, addition_large)
    {
        const auto a = integer{"50505050505050505050505050505050"};
        const auto b = integer{"51515151515151515151515151515151"};
        const auto s = integer{"A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1"};
        EXPECT_EQ(a + b, s);
    }

    TEST(integer_ossl, addition_small_with_assignment)
    {
        auto a = integer{2};
        a += 3;
        EXPECT_EQ(a, 5);
    }

    TEST(integer_ossl, addition_small_with_reusing_result)
    {
        auto a = integer{2};
        a = a + 3;
        EXPECT_EQ(a, 5);
    }

    TEST(integer_ossl, xor_different)
    {
        const auto a = integer{5};
        const auto b = integer{3};
        EXPECT_EQ(a ^ b, 6);
    }

    TEST(integer_ossl, division)
    {
        const auto a = integer{42};
        const auto b = integer{24};
        EXPECT_EQ(a / b, 1);
        EXPECT_EQ(a % b, 18);
    }

    TEST(integer_ossl, xor_with_itself)
    {
        const auto a = integer{42};
        EXPECT_EQ(a ^ a, 0);
    }

    TEST(integer_ossl, xor_with_itself_and_assign)
    {
        auto a = integer{42};
        a ^= a;
        EXPECT_EQ(a, 0);
    }

    TEST(integer_ossl, equals_to_itself)
    {
        const auto x = integer{42};
        EXPECT_EQ(x, x);
        EXPECT_TRUE(x >= x);
        EXPECT_TRUE(x <= x);
    }

    TEST(integer_ossl, not_equals_plus_one)
    {
        const auto x = integer{42};
        EXPECT_NE(x, x + 1);
    }

    TEST(integer_ossl, plus_one_is_greater)
    {
        const auto x = integer{42};
        EXPECT_TRUE(x + 1 > x);
        EXPECT_TRUE(x + 1 >= x);
    }

    TEST(integer_ossl, smaller_than_plus_one)
    {
        const auto x = integer{42};
        EXPECT_TRUE(x < x + 1);
        EXPECT_TRUE(x <= x + 1);
    }

    TEST(integer_ossl, multiplication_by_2_is_two_additions)
    {
        const auto a = integer{0x1234567890ABCD13};
        EXPECT_EQ(a + a, 2 * a);
    }

    TEST(integer_ossl, multiplication_by_3_is_three_additions)
    {
        const auto a = integer{0x1234567890ABCD13};
        EXPECT_EQ(a + a + a, 3 * a);
    }

    TEST(integer_ossl, multiplication_with_assignment)
    {
        auto a = integer{11};
        a *= 3;
        EXPECT_EQ(a, 33);
    }

    TEST(integer_ossl, bit_shift_left)
    {
        const auto a = integer{3};
        EXPECT_EQ(a << 2, 0xC);
    }

    TEST(integer_ossl, bit_shift_left_with_assignment)
    {
        auto a = integer{3};
        a <<= 2;
        EXPECT_EQ(a, 0xC);
    }

    TEST(integer_ossl, size_in_bytes)
    {
        auto x = integer{"02100D86957331832B8E8C230F5BD6A332B3615ACA"};
        EXPECT_GE(x.size_in_bytes(), 21);
    }

    TEST(integer_ossl, ostream)
    {
        std::stringstream stream;
        stream << std::hex << integer{"02100D86957331832B8E8C230F5BD6A332B3615ACA"};
        EXPECT_EQ("02100D86957331832B8E8C230F5BD6A332B3615ACA"s, stream.str());
    }

    TEST(integer_ossl, copy_assign)
    {
        auto x = integer{"02100D86957331832B8E8C230F5BD6A332B3615ACA"};
        auto y = x;
        EXPECT_EQ(x, y);

        auto z = integer{42};
        ASSERT_NE(x, z);
        z = x;
        EXPECT_EQ(x, z);
    }
}

namespace test_bmp
{
    using dstu4145::bmp::integer;

    TEST(integer_bmp, default_constructor)
    {
        EXPECT_EQ(integer{}, 0);
    }

    TEST(integer_bmp, from_long_and_default)
    {
        EXPECT_NE(integer{}, 1);
    }

    TEST(integer_bmp, from_long)
    {
        EXPECT_NE(integer{42}, 1);
        EXPECT_EQ(integer{42}, 42);
    }

    TEST(integer_bmp, int_to_buffer)
    {
        const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
        const auto expected = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

        auto rbuf = std::vector<std::byte>{};
        r.to_buffer(std::back_inserter(rbuf));

        EXPECT_EQ(rbuf, expected);
    }

    TEST(integer_bmp, construct_from_buffer)
    {
        const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
        const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

        EXPECT_EQ(r, integer{rbuf});
    }

    TEST(integer_bmp, construct_from_buffer_iterators)
    {
        const auto r = integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
        const auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

        EXPECT_EQ(r, integer(rbuf.begin(), rbuf.end()));
    }

    TEST(integer_bmp, bit_set_first)
    {
        auto x = integer{};
        x.bit_set(0);

        EXPECT_EQ(x, 1);
    }

    TEST(integer_bmp, bit_set_middle)
    {
        auto x = integer{};
        x.bit_set(3);
        x.bit_set(2);

        EXPECT_EQ(x, 0xC);
    }

    TEST(integer_bmp, bit_unset_first)
    {
        auto x = integer{1};
        x.bit_unset(0);

        EXPECT_EQ(x, 0);
    }

    TEST(integer_bmp, bit_unset_beyond_msb)
    {
        auto x = integer{1};
        x.bit_unset(1024);

        EXPECT_EQ(x, 1);
    }

    TEST(integer_bmp, bit_set_beyond_msb)
    {
        auto x = integer{1};
        x.bit_set(x.msb() + 1);

        EXPECT_EQ(x, 3);
    }

    TEST(integer_bmp, bit_unset_middle)
    {
        auto x = integer{0xF};
        x.bit_unset(3);
        x.bit_unset(2);

        EXPECT_EQ(x, 3);
    }

    TEST(integer_bmp, bit_unset_zero)
    {
        auto x = integer{0};
        x.bit_unset(0);

        EXPECT_EQ(x, 0);
    }

TEST(integer_bmp, bit_test)
    {
        auto x = integer{5};
        EXPECT_TRUE (x.bit_test(0));
        EXPECT_FALSE(x.bit_test(1));
        EXPECT_TRUE (x.bit_test(2));
    }

    TEST(integer_bmp, msb_lsb_5)
    {
        auto x = integer{5};
        EXPECT_EQ(x.msb(), 2);
        EXPECT_EQ(x.lsb(), 0);
    }

    TEST(integer_bmp, msb_lsb_1)
    {
        auto x = integer{1};
        EXPECT_EQ(x.msb(), 0);
        EXPECT_EQ(x.lsb(), 0);
    }

    TEST(integer_bmp, msb_lsb_0)
    {
        auto x = integer{0};
        EXPECT_THROW(x.msb(), std::exception);
        EXPECT_THROW(x.lsb(), std::exception);
    }

    TEST(integer_bmp, msb_lsb_2)
    {
        auto x = integer{2};
        EXPECT_EQ(x.msb(), 1);
        EXPECT_EQ(x.lsb(), 1);
    }

    TEST(integer_bmp, addition_small)
    {
        const auto a = integer{2};
        const auto b = integer{3};
        EXPECT_EQ(a + b, 5);
    }

    TEST(integer_bmp, addition_large)
    {
        const auto a = integer{"50505050505050505050505050505050"};
        const auto b = integer{"51515151515151515151515151515151"};
        const auto s = integer{"A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1A1"};
        EXPECT_EQ(a + b, s);
    }

    TEST(integer_bmp, addition_small_with_assignment)
    {
        auto a = integer{2};
        a += 3;
        EXPECT_EQ(a, 5);
    }

    TEST(integer_bmp, addition_small_with_reusing_result)
    {
        auto a = integer{2};
        a = a + 3;
        EXPECT_EQ(a, 5);
    }

    TEST(integer_bmp, xor_different)
    {
        const auto a = integer{5};
        const auto b = integer{3};
        EXPECT_EQ(a ^ b, 6);
    }

    TEST(integer_bmp, division)
    {
        const auto a = integer{42};
        const auto b = integer{24};
        EXPECT_EQ(a / b, 1);
        EXPECT_EQ(a % b, 18);
    }

    TEST(integer_bmp, xor_with_itself)
    {
        const auto a = integer{42};
        EXPECT_EQ(a ^ a, 0);
    }

    TEST(integer_bmp, xor_with_itself_and_assign)
    {
        auto a = integer{42};
        a ^= a;
        EXPECT_EQ(a, 0);
    }

    TEST(integer_bmp, equals_to_itself)
    {
        const auto x = integer{42};
        EXPECT_EQ(x, x);
        EXPECT_TRUE(x >= x);
        EXPECT_TRUE(x <= x);
    }

    TEST(integer_bmp, not_equals_plus_one)
    {
        const auto x = integer{42};
        EXPECT_NE(x, x + 1);
    }

    TEST(integer_bmp, plus_one_is_greater)
    {
        const auto x = integer{42};
        EXPECT_TRUE(x + 1 > x);
        EXPECT_TRUE(x + 1 >= x);
    }

    TEST(integer_bmp, smaller_than_plus_one)
    {
        const auto x = integer{42};
        EXPECT_TRUE(x < x + 1);
        EXPECT_TRUE(x <= x + 1);
    }

    TEST(integer_bmp, multiplication_by_2_is_two_additions)
    {
        const auto a = integer{0x1234567890ABCD13};
        EXPECT_EQ(a + a, 2 * a);
    }

    TEST(integer_bmp, multiplication_by_3_is_three_additions)
    {
        const auto a = integer{0x1234567890ABCD13};
        EXPECT_EQ(a + a + a, 3 * a);
    }

    TEST(integer_bmp, multiplication_with_assignment)
    {
        auto a = integer{11};
        a *= 3;
        EXPECT_EQ(a, 33);
    }

    TEST(integer_bmp, bit_shift_left)
    {
        const auto a = integer{3};
        EXPECT_EQ(a << 2, 0xC);
    }

    TEST(integer_bmp, bit_shift_left_with_assignment)
    {
        auto a = integer{3};
        a <<= 2;
        EXPECT_EQ(a, 0xC);
    }

    TEST(integer_bmp, size_in_bytes)
    {
        auto x = integer{"02100D86957331832B8E8C230F5BD6A332B3615ACA"};
        EXPECT_GE(x.size_in_bytes(), 21);
    }

    TEST(integer_bmp, ostream)
    {
        std::stringstream stream;
        stream << std::hex << integer{"02100D86957331832B8E8C230F5BD6A332B3615ACA"};
        EXPECT_EQ("2100d86957331832b8e8c230f5bd6a332b3615aca"s, stream.str());
    }

    TEST(integer_bmp, copy_assign)
    {
        auto x = integer{"02100D86957331832B8E8C230F5BD6A332B3615ACA"};
        auto y = x;
        EXPECT_EQ(x, y);

        auto z = integer{42};
        ASSERT_NE(x, z);
        z = x;
        EXPECT_EQ(x, z);
    }
}
