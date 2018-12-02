#include <gtest/gtest.h>

#include "ecurve.h"

using namespace std::literals;
using namespace testing;

// Test case from DSTU 4145-2002 Appendix B
struct curve163 : Test
{
    dstu4145::gf2m field{163, 7, 6, 3 };

    dstu4145::ecurve curve{
        field,
        1,
        dstu4145::integer{"5FF6108462A2DC8210AB403925E638A19C1455D21"}
    };

    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"72D867F93A93AC27DF9FF01AFFE74885C8C540420"},
        dstu4145::integer{"0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"}
    };

    dstu4145::integer d{"183F60FDF7951FF47D67193F8D073790C1C9B5A3E"};
    dstu4145::integer n{"400000000000000000002BEC12BE2262D39BCF14D"};
};

TEST_F(curve163, point_multiplication)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"57DE7FDE023FF929CB6AC785CE4B79CF64ABDC2DA"},
        dstu4145::integer{"695B3B9D26830943133078EF19FE8A8814F8F7B70"}
    };
    auto q = d * p;
    EXPECT_EQ(q, expected);
}

TEST_F(curve163, point_multiplication_result_is_not_infinity)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"759190F459A9C886F400D35F67AE6484577F1579A"},
        dstu4145::integer{"2594BA98D7357B01A8ADFECC0E519A38AB93B8BBC"}
    };
    auto q = n * p;
    EXPECT_NE(q, curve.infinity_point());
}

TEST_F(curve163, point_addition_equal)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"5A4397672F0C513E390212AD3825420BE83E3BFDA"},
        dstu4145::integer{"378C6CADAC80077C50EC218AB8C96015750C83564"}
    };
    auto q = p + p;
    EXPECT_EQ(q, expected);
}

TEST_F(curve163, point_addition_not_equal)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"61DA3389DDA324607BFFCC1EAD469465D033EDE07"},
        dstu4145::integer{"142B39F71B08662634682D26D3A1F4BE543B393E9"}
    };

    auto r = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"5A4397672F0C513E390212AD3825420BE83E3BFDA"},
        dstu4145::integer{"378C6CADAC80077C50EC218AB8C96015750C83564"}
    };
    auto q = r + p;
    EXPECT_EQ(q, expected);
}

TEST_F(curve163, point_addition_not_equal_pluseq)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"61DA3389DDA324607BFFCC1EAD469465D033EDE07"},
        dstu4145::integer{"142B39F71B08662634682D26D3A1F4BE543B393E9"}
    };

    auto r = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"5A4397672F0C513E390212AD3825420BE83E3BFDA"},
        dstu4145::integer{"378C6CADAC80077C50EC218AB8C96015750C83564"}
    };

    r+= p;
    EXPECT_EQ(r, expected);
}

TEST_F(curve163, public_key_computation)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"57DE7FDE023FF929CB6AC785CE4B79CF64ABDC2DA"},
        dstu4145::integer{"3E85444324BCF06AD85ABF6AD7B5F34770532B9AA"}
    };
    auto q = -(d * p);

    EXPECT_EQ(q, expected);
}

TEST_F(curve163, hash_to_field_element)
{
    auto hash = ::dstu4145::integer{"09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF"};
    auto h = field.create_element(hash);

    EXPECT_EQ(static_cast<dstu4145::integer>(h), dstu4145::integer{"03A2EB95B7180166DDF73532EEB76EDAEF52247FF"});
}

TEST_F(curve163, presignature_calculation)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"42A7D756D70E1C9BA62D2CB43707C35204EF3C67C"},
        dstu4145::integer{"5310AE5E560464A95DC80286F17EB762EC544B15B"}
    };

    auto e = dstu4145::integer{"1025E40BD97DB012B7A1D79DE8E12932D247F61C6"};
    auto ep = e * p;
    EXPECT_EQ(ep, expected);

    auto fe = static_cast<dstu4145::integer>(ep.x);
    EXPECT_EQ(fe, dstu4145::integer{"42A7D756D70E1C9BA62D2CB43707C35204EF3C67C"});
}

TEST_F(curve163, signature_calculation)
{
    auto hash = ::dstu4145::integer{"09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF"};

    auto e = dstu4145::integer{"1025E40BD97DB012B7A1D79DE8E12932D247F61C6"};
    auto fe = (e * p).x;

    auto h = field.create_element(hash);
    auto r = static_cast<dstu4145::integer>(h * fe);

    auto dr = (d * r) % n;
    auto s = (e + dr) % n;

    EXPECT_EQ(r, dstu4145::integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"});
    EXPECT_EQ(s, dstu4145::integer{"2100D86957331832B8E8C230F5BD6A332B3615ACA"});
}

TEST_F(curve163, signature_verification)
{
    auto q = -(d * p);

    auto hash = ::dstu4145::integer{"09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF"};

    auto h = field.create_element(hash);
    auto r = dstu4145::integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto s = dstu4145::integer{"2100D86957331832B8E8C230F5BD6A332B3615ACA"};

    auto rpoint = s * p + r * q;

    auto y = h * rpoint.x;

    EXPECT_EQ(static_cast<dstu4145::integer>(y), r);
}

TEST_F(curve163, find_point)
{
    auto actual = curve.find_point(dstu4145::integer{"72D867F93A93AC27DF9FF01AFFE74885C8C540420"});
    EXPECT_EQ(actual, p);
}


struct curve233 : Test
{
    dstu4145::ecurve curve{
        dstu4145::gf2m {233, 9, 4, 1 },
        1,
        dstu4145::integer{"06973B15095675534C7CF7E64A21BD54EF5DD3B8A0326AA936ECE454D2C"}
    };
    dstu4145::integer n {"1000000000000000000000000000013E974E72F8A6922031D2603CFE0D7"};
};

TEST_F(curve233, multiply_result_is_infinity)
{
    auto p = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"DA0B320C36CD1002DF8C7ED0B39CE6F34AA3FFCC265735590B55FDAC8E"},
        dstu4145::integer{"AAD851588085A83ABC90A7E4C3AD55246DF6C629EE15FB5D11D1CDC2DD"}
    };
    auto q = n * p;
    EXPECT_EQ(q, curve.infinity_point());
}