#include <gtest/gtest.h>

#include "polynomial.h"

TEST(polynomial, left_shift)
{
	using p = dstu4145::vec::polynomial;

    auto x = p{ dstu4145::integer{"8000908011a3"} };
    x.left_shift();
    EXPECT_EQ(x, p{ dstu4145::integer{"1000121002346"} });

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
        p{ dstu4145::integer{"1000023"} } * p{ dstu4145::integer{"1000102"} }, 
        p{ dstu4145::integer{"1000121002346"} }
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
    EXPECT_EQ(x, dstu4145::polynomial{ dstu4145::integer{"63"} });
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
