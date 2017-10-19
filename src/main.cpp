#include <gtest/gtest.h>

#include "ecurve.h"

using namespace testing;

struct simple : Test {
    dstu4145::gf2m field{163, 7, 6, 3 };

    dstu4145::ecurve curve{
        field,
        1,
        dstu4145::integer{"0x5FF6108462A2DC8210AB403925E638A19C1455D21" },
        dstu4145::integer{"0x400000000000000000002BEC12BE2262D39BCF14D" }
    };

    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"0x72D867F93A93AC27DF9FF01AFFE74885C8C540420" },
        dstu4145::integer{"0x0224A9C3947852B97C5599D5F4AB81122ADC3FD9B" }
    };

    dstu4145::integer d{"0x183F60FDF7951FF47D67193F8D073790C1C9B5A3E" };
};

TEST_F(simple, public_key_computation) {
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x057DE7FDE023FF929CB6AC785CE4B79CF64ABD2DA" },
        dstu4145::integer{"0x3E85444324BCF06AD85ABF6AD7B5F34770532B9AA" }
    };
    auto q = -(d * p);

    EXPECT_EQ(q, expected);
}

TEST_F(simple, point_multiplication) {
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x57DE7FDE023FF929CB6AC785CE4B79CF64ABDC2DA" },
        dstu4145::integer{"0x695B3B9D26830943133078EF19FE8A8814F8F7B70" }
    };
    auto q = d * p;
    EXPECT_EQ(q, expected);
}

TEST_F(simple, point_addition_equal) {
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x5A4397672F0C513E390212AD3825420BE83E3BFDA" },
        dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564" }
    };
    auto q = p + p;
    EXPECT_EQ(q, expected);
}

TEST_F(simple, point_addition_not_equal) {
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x61DA3389DDA324607BFFCC1EAD469465D033EDE07" },
        dstu4145::integer{"0x142B39F71B08662634682D26D3A1F4BE543B393E9" }
    };

    auto r = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x5A4397672F0C513E390212AD3825420BE83E3BFDA" },
        dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564" }
    };
    auto q = r + p;
    EXPECT_EQ(q, expected);
}

TEST_F(simple, field_element_addition_small) {
    auto a = field.create_element(1);
    auto b = field.create_element(2);

    EXPECT_EQ(a + b, field.create_element(3));
}

TEST_F(simple, field_element_addition_small_equal) {
    auto a = field.create_element(1);
    auto b = field.create_element(1);

    EXPECT_EQ(a + b, field.create_element(0));
}

TEST_F(simple, field_element_addition_big) {
    auto a = field.create_element(dstu4145::integer{"0x695B3B9D26830943133078EF19FE8A8814F8F7B70"});
    auto b = field.create_element(dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564"});

    EXPECT_EQ(a + b, field.create_element(dstu4145::integer{"0x5ED757308A030E3F43DC5965A137EA9D61F474E14"}));
}

TEST_F(simple, field_element_multiplication_small) {
    auto a = field.create_element(2);
    auto b = field.create_element(3);

    EXPECT_EQ(a * b, field.create_element(6));
}

TEST_F(simple, field_element_multiplication_big) {
    auto a = field.create_element(dstu4145::integer{"0x695B3B9D26830943133078EF19FE8A8814F8F7B70"});
    auto b = field.create_element(dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564"});

    EXPECT_EQ(a * b, field.create_element(dstu4145::integer{"0x2F44EF2885428821377088E7AB7110467B10B663B"}));
}

TEST(utils, modulo_small) {
    auto a = dstu4145::integer{"0x63"};
    auto b = dstu4145::integer{"0x2A"};

    EXPECT_EQ(dstu4145::p_modulo(a, b), dstu4145::integer{"0x1D"});

}

TEST_F(simple, hash_to_field_element) {
    auto hash = ::dstu4145::integer{"0x09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF" };
    auto h = field.create_element(hash);

    EXPECT_EQ(static_cast<dstu4145::integer>(h), dstu4145::integer{"0x03A2EB95B7180166DDF73532EEB76EDAEF52247FF" });
}


TEST(multiply, all) {
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

TEST(euqlid, simple) {
    auto [d, a, b] = dstu4145::extended_euqlid(6, 4, 6);
    a; b;

    EXPECT_EQ(d, dstu4145::integer{2});
}

TEST(gf2m, inverse) {
    auto field = dstu4145::gf2m{7, 5, 2, 1};
    auto element = dstu4145::gf2m_element{field, 42};
    auto inverted = dstu4145::gf2m_element{field, 62};

    EXPECT_EQ(element.inverse(), inverted);
}