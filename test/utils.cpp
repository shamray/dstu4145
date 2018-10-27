#include <gtest/gtest.h>

#include "polynomial.h"
#include "utils.h"

using namespace std::literals;

TEST(utils, modulo_small)
{
    auto a = dstu4145::polynomial{dstu4145::integer{"0x63"}};
    auto b = dstu4145::polynomial{dstu4145::integer{"0x2A"}};

    EXPECT_EQ(a % b, dstu4145::polynomial{dstu4145::integer{"0x1D"}});

}

using dstu4145::multiply;

TEST(utils, multiply_by_1)
{
    ASSERT_EQ(multiply(1,1), 1);
}

TEST(utils, multiply_by_3)
{
    ASSERT_EQ(multiply(3,1), 3);
}

TEST(utils, multiply_by_5)
{
    ASSERT_EQ(multiply(5,1), 5);
}

TEST(utils, multiply_by_4)
{
    ASSERT_EQ(multiply(4,1), 4);
}

TEST(utils, multiply_by_hundred)
{
    ASSERT_EQ(multiply(100,3), 300);
}

TEST(utils, euqlid_simple)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{6}, dstu4145::polynomial{4}, dstu4145::polynomial{6});
    a; b;

    EXPECT_EQ(d, dstu4145::polynomial{2});
}

TEST(utils, euqlid_base_case)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{6}, dstu4145::polynomial{0}, dstu4145::polynomial{6});
    EXPECT_EQ(d, dstu4145::polynomial{6});
    EXPECT_EQ(a, dstu4145::polynomial{1});
    EXPECT_EQ(b, dstu4145::polynomial{0});
}

TEST(utils, euqlid_base_case2)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{2}, dstu4145::polynomial{0}, dstu4145::polynomial{1});
    EXPECT_EQ(d, dstu4145::polynomial{2});
    EXPECT_EQ(a, dstu4145::polynomial{1});
    EXPECT_EQ(b, dstu4145::polynomial{0});
}

TEST(utils, euqlid_one_iteration)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{4}, dstu4145::polynomial{2}, dstu4145::polynomial{6});
    EXPECT_EQ(d, dstu4145::polynomial{2});
    EXPECT_EQ(a, dstu4145::polynomial{0});
    EXPECT_EQ(b, dstu4145::polynomial{1});
}

TEST(utils, euqlid_two_iterations)
{
    auto [d, a, b] = dstu4145::extended_euqlid(dstu4145::polynomial{6}, dstu4145::polynomial{4}, dstu4145::polynomial{6});
    EXPECT_EQ(d, dstu4145::polynomial{2});
    EXPECT_EQ(a, dstu4145::polynomial{1});
    EXPECT_EQ(b, dstu4145::polynomial{1});
}
