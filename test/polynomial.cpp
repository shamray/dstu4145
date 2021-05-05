#include <gtest/gtest.h>
#include "test-utils.h"

#include "polynomial.h"

TEST(polynomial, create_from_buffer_zero)
{
    auto b = hex_buffer("00");
    auto p = dstu4145::polynomial{ std::begin(b), std::end(b) };

    dstu4145::polynomial expected{0};

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_buffer_one)
{
    auto b = hex_buffer("01");
    auto p = dstu4145::polynomial{ std::begin(b), std::end(b) };

    dstu4145::polynomial expected;
    expected.bit_set(0);

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_buffer)
{
    auto b = hex_buffer("0120");
    auto p = dstu4145::polynomial{std::begin(b), std::end(b)};

    dstu4145::polynomial expected;
    expected.bit_set(5);
    expected.bit_set(8);

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_buffer_big)
{
    auto b = hex_buffer("0800000000000000000000000000000000000000C9");
    auto p = dstu4145::polynomial{ std::begin(b), std::end(b) };

    auto expected = dstu4145::polynomial{163, 7, 6, 3};

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_container)
{
    auto b = hex_buffer("0800000000000000000000000000000000000000C9");
    auto p = dstu4145::polynomial{ b };

    auto expected = dstu4145::polynomial{ 163, 7, 6, 3 };

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_hex_buffer_zero)
{
    auto p = dstu4145::polynomial{ "00" };
    auto expected = dstu4145::polynomial{ 0 };

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_hex_buffer_one)
{
    auto p = dstu4145::polynomial{ "01" };
    auto expected = dstu4145::polynomial{ 1 };

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_hex_buffer)
{
    auto p = dstu4145::polynomial{ "0120" };

    dstu4145::polynomial expected;
    expected.bit_set(5);
    expected.bit_set(8);

    EXPECT_EQ(p, expected);
}

TEST(polynomial, create_from_hex_buffer_big)
{
    auto p = dstu4145::polynomial{"0800000000000000000000000000000000000000C9"};
    auto expected = dstu4145::polynomial{163, 7, 6, 3};

    EXPECT_EQ(p, expected);
}

TEST(polynomial, left_shift)
{
    using p = dstu4145::vec::polynomial;

    auto x = p{"8000908011a3"};
    x.left_shift();
    EXPECT_EQ(x, p{"01000121002346"});

}
TEST(polynomial, multiplication)
{
    using p = dstu4145::vec::polynomial;

    EXPECT_EQ(p{ 1 } * p{ 1 }, p{ 1 });
    EXPECT_EQ(p{ 0 } * p{ 0 }, p{ 0 });
    EXPECT_EQ(p{ 1 } * p{ 0 }, p{ 0 });
    EXPECT_EQ(p{ 0 } * p{ 1 }, p{ 0 });

    EXPECT_EQ(p{ 1 } * p{ 42 }, p{ 42 });
    EXPECT_EQ(p{ 42 } * p{ 1 }, p{ 42 });
    EXPECT_EQ(p{ 0 } * p{ 42 }, p{ 0 });
    EXPECT_EQ(p{ 42 } * p{ 0 }, p{ 0 });

    EXPECT_EQ(p{ 42 } *p{ 2 }, p{ 84 });
    EXPECT_EQ(p{ 42 } *p{ 42 }, p{ 1092 });

    EXPECT_EQ(
        p{"01000023"} * p{"01000102"}, 
        p{"01000121002346"}
    );
}

TEST(polynomial, internal_chunk_size)
{
    EXPECT_EQ(dstu4145::vec::polynomial::internal_chunk_size(), 32);
}

TEST(polynomial, set_bit)
{
    auto x = dstu4145::polynomial{};
    x.bit_set(0);
    EXPECT_EQ(x, dstu4145::polynomial{1});
    EXPECT_EQ(x.msb(), 0);
}

TEST(polynomial, set_bit_several_bits)
{
    auto x = dstu4145::polynomial{};
    x.bit_set(0);
    x.bit_set(1);
    x.bit_set(5);
    x.bit_set(6);
    EXPECT_EQ(x.msb(), 6);
    EXPECT_EQ(x, dstu4145::polynomial{ 0x63 });
    EXPECT_EQ(x, dstu4145::polynomial{ "63" });
}

TEST(polynomial, unset_bit)
{
    auto x = dstu4145::polynomial{1};
    x.bit_unset(0);
    EXPECT_EQ(x, dstu4145::polynomial{});
}

TEST(polynomial, add_equal)
{
    auto x = dstu4145::polynomial{1} + dstu4145::polynomial{1};
    EXPECT_EQ(x, dstu4145::polynomial{});
}

TEST(polynomial, add_unequal)
{
    auto x = dstu4145::polynomial{1} + dstu4145::polynomial{2};
    EXPECT_EQ(x, dstu4145::polynomial{3});
}

TEST(polynomial, div) {
    auto [r, q] = dstu4145::polynomial{4} / dstu4145::polynomial{2};
    EXPECT_EQ(r, dstu4145::polynomial{0});
    EXPECT_EQ(q, dstu4145::polynomial{2});
}

TEST(polynomial, euqlid_base_case)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{6}, dstu4145::polynomial{0}, dstu4145::polynomial{6});
    EXPECT_EQ(d, dstu4145::polynomial{6});
    EXPECT_EQ(a, dstu4145::polynomial{1});
    EXPECT_EQ(b, dstu4145::polynomial{0});
}

TEST(polynomial, euqlid_base_case2)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{2}, dstu4145::polynomial{0}, dstu4145::polynomial{1});
    EXPECT_EQ(d, dstu4145::polynomial{2});
    EXPECT_EQ(a, dstu4145::polynomial{1});
    EXPECT_EQ(b, dstu4145::polynomial{0});
}

TEST(polynomial, euqlid_one_iteration)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{4}, dstu4145::polynomial{2}, dstu4145::polynomial{6});
    EXPECT_EQ(d, dstu4145::polynomial{2});
    EXPECT_EQ(a, dstu4145::polynomial{0});
    EXPECT_EQ(b, dstu4145::polynomial{1});
}

TEST(polynomial, euqlid_two_iterations)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{6}, dstu4145::polynomial{4}, dstu4145::polynomial{6});
    EXPECT_EQ(d, dstu4145::polynomial{2});
    EXPECT_EQ(a, dstu4145::polynomial{1});
    EXPECT_EQ(b, dstu4145::polynomial{1});
}

TEST(polynomial, modulo_small)
{
    auto a = dstu4145::polynomial{"63"};
    auto b = dstu4145::polynomial{"2A"};

    EXPECT_EQ(a % b, dstu4145::polynomial{"1D"});
}

TEST(polynomial, bit_set_empty)
{
    auto x = dstu4145::polynomial{};
    x.bit_set(0);

    EXPECT_EQ(x, dstu4145::polynomial{1});
}

TEST(polynomial, bit_set_small)
{
    auto x = dstu4145::polynomial{2};
    x.bit_set(0);

    EXPECT_EQ(x, dstu4145::polynomial{3});
}

TEST(polynomial, bit_unset_empty)
{
    auto x = dstu4145::polynomial{};
    x.bit_unset(0);

    EXPECT_EQ(x, dstu4145::polynomial{});
}

TEST(polynomial, bit_unset_small)
{
    auto x = dstu4145::polynomial{"FF"};
    x.bit_unset(0);
    
    EXPECT_EQ(x, dstu4145::polynomial{"FE"});
}

TEST(polynomial, to_buffer_one_byte)
{
    auto x = dstu4145::polynomial{"01"};
    auto expected = hex_buffer("01");

    std::vector<std::byte> actual;
    x.to_buffer(std::back_inserter(actual));
    
    EXPECT_EQ(actual, expected);
}

TEST(polynomial, to_buffer_one_byte_ff)
{
    auto x = dstu4145::polynomial{ "FF" };
    auto expected = hex_buffer("FF");

    std::vector<std::byte> actual;
    x.to_buffer(std::back_inserter(actual));

    EXPECT_EQ(actual, expected);
}

TEST(polynomial, to_buffer_two_bytes)
{
    auto x = dstu4145::polynomial{"0102"};
    auto expected = hex_buffer("0102");

    std::vector<std::byte> actual;
    x.to_buffer(std::back_inserter(actual));
    
    EXPECT_EQ(actual, expected);
}

TEST(polynomial, to_buffer_four_bytes)
{
    auto x = dstu4145::polynomial{"01020304"};
    auto expected = hex_buffer("01020304");

    std::vector<std::byte> actual;
    x.to_buffer(std::back_inserter(actual));
    
    EXPECT_EQ(actual, expected);
}

TEST(polynomial, to_buffer_five_bytes)
{
    auto x = dstu4145::polynomial{"0102030405"};
    auto expected = hex_buffer("0102030405");

    std::vector<std::byte> actual;
    x.to_buffer(std::back_inserter(actual));
    
    EXPECT_EQ(actual, expected);
}

TEST(polynomial, to_buffer_ten_bytes)
{
    auto x = dstu4145::polynomial{"0102030405060708090A"};
    auto expected = hex_buffer("0102030405060708090A");

    std::vector<std::byte> actual;
    x.to_buffer(std::back_inserter(actual));

    EXPECT_EQ(actual, expected);
}
