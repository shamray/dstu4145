#include <gtest/gtest.h>

#include "gf2m.h"
#include "gf2m_element.h"

using namespace testing;

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
    auto u = field.element(dstu4145::integer{"4AFB244364AA35D5A34F61A76615D1968E512F94"});
    auto w = u.trace();

    EXPECT_EQ(w, field.element(dstu4145::integer{0}));
}

TEST(gf2m_test, half_trace)
{
    auto field = dstu4145::gf2m{163, 7, 6, 3 };
    auto u = field.element(dstu4145::integer{"4AFB244364AA35D5A34F61A76615D1968E512F94"});
    auto w = u.half_trace();

    EXPECT_EQ(w, field.element(dstu4145::integer{"166D14411C58E39898B0A99989AE32BDD86B1E24"}));
}

TEST(gf2m_test, multiply_identical)
{
    auto field = dstu4145::gf2m{163, 7, 6, 3 };
    auto a = field.element(dstu4145::integer{"4CAD141C3B5ADA6B8A55A0A1FDA6D223A86F85B5E"});
    auto expected = field.element(dstu4145::integer{"1DC6F426DE35AEE2A90290E014D64E6AE3C4FA81E"});

    EXPECT_EQ(a * a, expected);
}

TEST(gf2m_test, add)
{
    auto field = dstu4145::gf2m{163, 7, 6, 3 };
    auto a = field.element(dstu4145::integer{"4CAD141C3B5ADA6B8A55A0A1FDA6D223A86F85B5E"});
    auto s = field.element(dstu4145::integer{"1DC6F426DE35AEE2A90290E014D64E6AE3C4FA81E"});
    auto expected = field.element(dstu4145::integer{"516BE03AE56F748923573041E9709C494BAB7F340"});

    EXPECT_EQ(a + s, expected);

    auto r = s + a;
}

struct field163 : Test
{
    dstu4145::gf2m field{163, 7, 6, 3 };
};

TEST_F(field163, field_element_addition_small)
{
    auto a = field.element(1);
    auto b = field.element(2);

    EXPECT_EQ(a + b, field.element(3));
}

TEST_F(field163, field_element_addition_small_equal)
{
    auto a = field.element(1);
    auto b = field.element(1);

    EXPECT_EQ(a + b, field.element(0));
}

TEST_F(field163, field_element_addition_big)
{
    auto a = field.element(dstu4145::integer{"695B3B9D26830943133078EF19FE8A8814F8F7B70"});
    auto b = field.element(dstu4145::integer{"378C6CADAC80077C50EC218AB8C96015750C83564"});

    EXPECT_EQ(a + b, field.element(dstu4145::integer{"5ED757308A030E3F43DC5965A137EA9D61F474E14"}));
}

TEST_F(field163, field_element_multiplication_small)
{
    auto a = field.element(2);
    auto b = field.element(3);

    EXPECT_EQ(a * b, field.element(6));
}

TEST_F(field163, field_element_multiplication_big) {
    auto a = field.element(dstu4145::integer{"695B3B9D26830943133078EF19FE8A8814F8F7B70"});
    auto b = field.element(dstu4145::integer{"378C6CADAC80077C50EC218AB8C96015750C83564"});

    EXPECT_EQ(a * b, field.element(dstu4145::integer{"2F44EF2885428821377088E7AB7110467B10B663B"}));
}
