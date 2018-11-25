#include <gtest/gtest.h>

#include "utils.h"

using namespace std::literals;

TEST(utils, modulo_small)
{
    auto a = dstu4145::polynomial{dstu4145::integer{"63"}};
    auto b = dstu4145::polynomial{dstu4145::integer{"2A"}};

    EXPECT_EQ(a % b, dstu4145::polynomial{dstu4145::integer{"1D"}});

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
