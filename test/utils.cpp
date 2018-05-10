#include <gtest/gtest.h>

#include "utils.h"

using namespace std::literals;

TEST(utils, modulo_small)
{
    auto a = dstu4145::integer{"0x63"};
    auto b = dstu4145::integer{"0x2A"};

    EXPECT_EQ(dstu4145::p_modulo(a, b), dstu4145::integer{"0x1D"});

}

TEST(multiply, all)
{
    using dstu4145::multiply;

    EXPECT_EQ(multiply(100, 3), 300);

    EXPECT_EQ(multiply(1, 10), 10);
    EXPECT_EQ(multiply(2, 10), 20);
    EXPECT_EQ(multiply(3, 10), 30);
    EXPECT_EQ(multiply(4, 10), 40);
    EXPECT_EQ(multiply(5, 10), 50);
    EXPECT_EQ(multiply(6, 10), 60);
    EXPECT_EQ(multiply(7, 10), 70);
    EXPECT_EQ(multiply(8, 10), 80);
    EXPECT_EQ(multiply(9, 10), 90);
    EXPECT_EQ(multiply(10, 10), 100);
    EXPECT_EQ(multiply(11, 10), 110);
    EXPECT_EQ(multiply(12, 10), 120);
    EXPECT_EQ(multiply(13, 10), 130);
    EXPECT_EQ(multiply(14, 10), 140);
    EXPECT_EQ(multiply(15, 10), 150);
}

TEST(euqlid, simple)
{
    auto [d, a, b] = dstu4145::extended_euqlid(6, 4, 6);
    a; b;

    EXPECT_EQ(d, dstu4145::integer{2});
}
