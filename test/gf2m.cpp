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

TEST(gf2m_test, trace)
{
    auto field = dstu4145::gf2m{163, 7, 6, 3 };
    auto u = field.create_element(dstu4145::integer{"0x4AFB244364AA35D5A34F61A76615D1968E512F94"});
    auto w = u.trace();
}

TEST(gf2m_test, multiply_identical)
{
    auto field = dstu4145::gf2m{163, 7, 6, 3 };
    auto a = field.create_element(dstu4145::integer{"0x4CAD141C3B5ADA6B8A55A0A1FDA6D223A86F85B5E"});
    auto s = a * a;
    std::cout << std::endl << s << std::endl;
}

TEST(gf2m_test, add)
{
    auto field = dstu4145::gf2m{163, 7, 6, 3 };
    auto a = field.create_element(dstu4145::integer{"0x4CAD141C3B5ADA6B8A55A0A1FDA6D223A86F85B5E"});
    auto s = field.create_element(dstu4145::integer{"0x1DC6F426DE35AEE2A90290E014D64E6AE3C4FA81E"});
    auto expected = field.create_element(dstu4145::integer{"0x516BE03AE56F748923573041E9709C494BAB7F340"});

    EXPECT_EQ(a + s, expected);

    auto r = s + a;
}
