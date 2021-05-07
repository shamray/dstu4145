#include <gtest/gtest.h>
#include "test-utils.h"

#include "polynomial.h"

namespace test_vec
{
    using dstu4145::vec::polynomial;
    
    TEST(polynomial_vec, create_from_buffer_zero)
    {
        auto b = hex_buffer("00");
        auto p = polynomial{ std::begin(b), std::end(b) };

        polynomial expected{0};

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_buffer_one)
    {
        auto b = hex_buffer("01");
        auto p = polynomial{ std::begin(b), std::end(b) };

        polynomial expected;
        expected.bit_set(0);

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_buffer)
    {
        auto b = hex_buffer("0120");
        auto p = polynomial{std::begin(b), std::end(b)};

        polynomial expected;
        expected.bit_set(5);
        expected.bit_set(8);

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_buffer_big)
    {
        auto b = hex_buffer("0800000000000000000000000000000000000000C9");
        auto p = polynomial{ std::begin(b), std::end(b) };

        auto expected = polynomial{163, 7, 6, 3};

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_container)
    {
        auto b = hex_buffer("0800000000000000000000000000000000000000C9");
        auto p = polynomial{ b };

        auto expected = polynomial{ 163, 7, 6, 3 };

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_hex_buffer_zero)
    {
        auto p = polynomial{ "00" };
        auto expected = polynomial{ 0 };

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_hex_buffer_one)
    {
        auto p = polynomial{ "01" };
        auto expected = polynomial{ 1 };

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_hex_buffer)
    {
        auto p = polynomial{ "0120" };

        polynomial expected;
        expected.bit_set(5);
        expected.bit_set(8);

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, create_from_hex_buffer_big)
    {
        auto p = polynomial{"0800000000000000000000000000000000000000C9"};
        auto expected = polynomial{163, 7, 6, 3};

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_vec, left_shift)
    {
        auto x = polynomial{"8000908011a3"};
        x.left_shift();
        EXPECT_EQ(x, polynomial{"01000121002346"});

    }

    TEST(polynomial_vec, multiplication)
    {
        using p = polynomial;

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

    TEST(polynomial_vec, internal_chunk_size)
    {
        EXPECT_EQ(polynomial::internal_chunk_size(), 32);
    }

    TEST(polynomial_vec, set_bit)
    {
        auto x = polynomial{};
        x.bit_set(0);
        EXPECT_EQ(x, polynomial{1});
        EXPECT_EQ(x.msb(), 0);
    }

    TEST(polynomial_vec, set_bit_several_bits)
    {
        auto x = polynomial{};
        x.bit_set(0);
        x.bit_set(1);
        x.bit_set(5);
        x.bit_set(6);
        EXPECT_EQ(x.msb(), 6);
        EXPECT_EQ(x, polynomial{ 0x63 });
        EXPECT_EQ(x, polynomial{ "63" });
    }

    TEST(polynomial_vec, unset_bit)
    {
        auto x = polynomial{1};
        x.bit_unset(0);
        EXPECT_EQ(x, polynomial{});
    }

    TEST(polynomial_vec, add_equal)
    {
        auto x = polynomial{1} + polynomial{1};
        EXPECT_EQ(x, polynomial{});
    }

    TEST(polynomial_vec, add_unequal)
    {
        auto x = polynomial{1} + polynomial{2};
        EXPECT_EQ(x, polynomial{3});
    }

    TEST(polynomial_vec, div) {
        auto [r, q] = polynomial{4} / polynomial{2};
        EXPECT_EQ(r, polynomial{0});
        EXPECT_EQ(q, polynomial{2});
    }

    TEST(polynomial_vec, modulo_small)
    {
        auto a = polynomial{"63"};
        auto b = polynomial{"2A"};

        EXPECT_EQ(a % b, polynomial{"1D"});
    }

    TEST(polynomial_vec, bit_set_empty)
    {
        auto x = polynomial{};
        x.bit_set(0);

        EXPECT_EQ(x, polynomial{1});
    }

    TEST(polynomial_vec, bit_set_small)
    {
        auto x = polynomial{2};
        x.bit_set(0);

        EXPECT_EQ(x, polynomial{3});
    }

    TEST(polynomial_vec, bit_unset_empty)
    {
        auto x = polynomial{};
        x.bit_unset(0);

        EXPECT_EQ(x, polynomial{});
    }

    TEST(polynomial_vec, bit_unset_small)
    {
        auto x = polynomial{"FF"};
        x.bit_unset(0);
    
        EXPECT_EQ(x, polynomial{"FE"});
    }

    TEST(polynomial_vec, to_buffer_one_byte)
    {
        auto x = polynomial{"01"};
        auto expected = hex_buffer("01");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_vec, to_buffer_one_byte_ff)
    {
        auto x = polynomial{ "FF" };
        auto expected = hex_buffer("FF");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));

        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_vec, to_buffer_two_bytes)
    {
        auto x = polynomial{"0102"};
        auto expected = hex_buffer("0102");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_vec, to_buffer_four_bytes)
    {
        auto x = polynomial{"01020304"};
        auto expected = hex_buffer("01020304");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_vec, to_buffer_five_bytes)
    {
        auto x = polynomial{"0102030405"};
        auto expected = hex_buffer("0102030405");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_vec, to_buffer_ten_bytes)
    {
        auto x = polynomial{"0102030405060708090A"};
        auto expected = hex_buffer("0102030405060708090A");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));

        EXPECT_EQ(actual, expected);
    }
}

namespace test_int
{
    using dstu4145::in::polynomial;
    
    TEST(polynomial_int, create_from_buffer_zero)
    {
        auto b = hex_buffer("00");
        auto p = polynomial{ std::begin(b), std::end(b) };

        polynomial expected{0};

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_buffer_one)
    {
        auto b = hex_buffer("01");
        auto p = polynomial{ std::begin(b), std::end(b) };

        polynomial expected;
        expected.bit_set(0);

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_buffer)
    {
        auto b = hex_buffer("0120");
        auto p = polynomial{std::begin(b), std::end(b)};

        polynomial expected;
        expected.bit_set(5);
        expected.bit_set(8);

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_buffer_big)
    {
        auto b = hex_buffer("0800000000000000000000000000000000000000C9");
        auto p = polynomial{ std::begin(b), std::end(b) };

        auto expected = polynomial{163, 7, 6, 3};

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_container)
    {
        auto b = hex_buffer("0800000000000000000000000000000000000000C9");
        auto p = polynomial{ b };

        auto expected = polynomial{ 163, 7, 6, 3 };

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_hex_buffer_zero)
    {
        auto p = polynomial{"00"};
        auto expected = polynomial{ 0 };

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_hex_buffer_one)
    {
        auto p = polynomial{"01"};
        auto expected = polynomial{ 1 };

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_hex_buffer)
    {
        auto p = polynomial{"0120"};

        polynomial expected;
        expected.bit_set(5);
        expected.bit_set(8);

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, create_from_hex_buffer_big)
    {
        auto p = polynomial{"0800000000000000000000000000000000000000C9"};
        auto expected = polynomial{163, 7, 6, 3};

        EXPECT_EQ(p, expected);
    }

    TEST(polynomial_int, multiplication)
    {
        using p = polynomial;

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

    TEST(polynomial_int, set_bit)
    {
        auto x = polynomial{};
        x.bit_set(0);
        EXPECT_EQ(x, polynomial{1});
        EXPECT_EQ(x.msb(), 0);
    }

    TEST(polynomial_int, set_bit_several_bits)
    {
        auto x = polynomial{};
        x.bit_set(0);
        x.bit_set(1);
        x.bit_set(5);
        x.bit_set(6);
        EXPECT_EQ(x.msb(), 6);
        EXPECT_EQ(x, polynomial{ 0x63 });
        EXPECT_EQ(x, polynomial{ "63" });
    }

    TEST(polynomial_int, unset_bit)
    {
        auto x = polynomial{1};
        x.bit_unset(0);
        EXPECT_EQ(x, polynomial{});
    }

    TEST(polynomial_int, add_equal)
    {
        auto x = polynomial{1} + polynomial{1};
        EXPECT_EQ(x, polynomial{});
    }

    TEST(polynomial_int, add_unequal)
    {
        auto x = polynomial{1} + polynomial{2};
        EXPECT_EQ(x, polynomial{3});
    }

    TEST(polynomial_int, div) {
        auto [r, q] = polynomial{4} / polynomial{2};
        EXPECT_EQ(r, polynomial{0});
        EXPECT_EQ(q, polynomial{2});
    }

    TEST(polynomial_int, modulo_small)
    {
        auto a = polynomial{"63"};
        auto b = polynomial{"2A"};

        EXPECT_EQ(a % b, polynomial{"1D"});
    }

    TEST(polynomial_int, bit_set_empty)
    {
        auto x = polynomial{};
        x.bit_set(0);

        EXPECT_EQ(x, polynomial{1});
    }

    TEST(polynomial_int, bit_set_small)
    {
        auto x = polynomial{2};
        x.bit_set(0);

        EXPECT_EQ(x, polynomial{3});
    }

    TEST(polynomial_int, bit_unset_empty)
    {
        auto x = polynomial{};
        x.bit_unset(0);

        EXPECT_EQ(x, polynomial{});
    }

    TEST(polynomial_int, bit_unset_small)
    {
        auto x = polynomial{"FF"};
        x.bit_unset(0);
    
        EXPECT_EQ(x, polynomial{"FE"});
    }

    /* TODO: implement to_buffer() and uncomment
    TEST(polynomial_int, to_buffer_one_byte)
    {
        auto x = polynomial{"01"};
        auto expected = hex_buffer("01");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_int, to_buffer_one_byte_ff)
    {
        auto x = polynomial{ "FF" };
        auto expected = hex_buffer("FF");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));

        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_int, to_buffer_two_bytes)
    {
        auto x = polynomial{"0102"};
        auto expected = hex_buffer("0102");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_int, to_buffer_four_bytes)
    {
        auto x = polynomial{"01020304"};
        auto expected = hex_buffer("01020304");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_int, to_buffer_five_bytes)
    {
        auto x = polynomial{"0102030405"};
        auto expected = hex_buffer("0102030405");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));
    
        EXPECT_EQ(actual, expected);
    }

    TEST(polynomial_int, to_buffer_ten_bytes)
    {
        auto x = polynomial{"0102030405060708090A"};
        auto expected = hex_buffer("0102030405060708090A");

        std::vector<std::byte> actual;
        x.to_buffer(std::back_inserter(actual));

        EXPECT_EQ(actual, expected);
    }
    */
}

namespace test_polynomial
{
    using dstu4145::polynomial;

    TEST(polynomial, euqlid_base_case)
    {
        auto [d, a, b] = dstu4145::extended_euqlid(polynomial{6}, polynomial{0}, polynomial{6});
        EXPECT_EQ(d, polynomial{6});
        EXPECT_EQ(a, polynomial{1});
        EXPECT_EQ(b, polynomial{0});
    }

    TEST(polynomial, euqlid_base_case2)
    {
        auto [d, a, b] = dstu4145::extended_euqlid(polynomial{2}, polynomial{0}, polynomial{1});
        EXPECT_EQ(d, polynomial{2});
        EXPECT_EQ(a, polynomial{1});
        EXPECT_EQ(b, polynomial{0});
    }

    TEST(polynomial, euqlid_one_iteration)
    {
        auto [d, a, b] = dstu4145::extended_euqlid(polynomial{4}, polynomial{2}, polynomial{6});
        EXPECT_EQ(d, polynomial{2});
        EXPECT_EQ(a, polynomial{0});
        EXPECT_EQ(b, polynomial{1});
    }

    TEST(polynomial, euqlid_two_iterations)
    {
        auto [d, a, b] = dstu4145::extended_euqlid(polynomial{6}, polynomial{4}, polynomial{6});
        EXPECT_EQ(d, polynomial{2});
        EXPECT_EQ(a, polynomial{1});
        EXPECT_EQ(b, polynomial{1});
    }
}