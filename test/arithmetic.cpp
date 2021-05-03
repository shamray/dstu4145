#include <gtest/gtest.h>

#include "arithmetic.h"

using dstu4145::multiply;

TEST(arithmetic, multiply_by_1)
{
    ASSERT_EQ(multiply(1,1), 1);
}

TEST(arithmetic, multiply_by_3)
{
    ASSERT_EQ(multiply(3,1), 3);
}

TEST(arithmetic, multiply_by_5)
{
    ASSERT_EQ(multiply(5,1), 5);
}

TEST(arithmetic, multiply_by_4)
{
    ASSERT_EQ(multiply(4,1), 4);
}

TEST(arithmetic, multiply_by_hundred)
{
    ASSERT_EQ(multiply(100,3), 300);
}
