#include <gtest/gtest.h>

#include "gf2m.h"
#include "gf2m_element.h"

TEST(gf2m_test, inverse)
{
    auto field = dstu4145::gf2m{7, 5, 2, 1};
    auto element = dstu4145::gf2m_element{field, 42};
    auto inverted = dstu4145::gf2m_element{field, 62};

    EXPECT_EQ(element.inverse(), inverted);
}
