#include <gtest/gtest.h>

#include "ecurve.h"
#include "test-utils.h"

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

TEST_F(curve163, point_multiplication_result_is_infinity)
{
    auto q = n * p;
    EXPECT_EQ(q, curve.infinity_point());
}

TEST_F(curve163, point_multiplication_result_is_not_infinity)
{
    auto p = dstu4145::ecurve::point{
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
    auto hash = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto h = field.create_element(dstu4145::polynomial{hash});

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
    auto hash = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");

    auto e = dstu4145::integer{"1025E40BD97DB012B7A1D79DE8E12932D247F61C6"};
    auto fe = (e * p).x;

    auto h = field.create_element(dstu4145::polynomial{hash});
    auto r = static_cast<dstu4145::integer>(h * fe);

    auto dr = (d * r) % n;
    auto s = (e + dr) % n;

    EXPECT_EQ(r, dstu4145::integer{"274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"});
    EXPECT_EQ(s, dstu4145::integer{"2100D86957331832B8E8C230F5BD6A332B3615ACA"});
}

TEST_F(curve163, signature_verification)
{
    auto q = -(d * p);

    auto hash = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");

    auto h = field.create_element(dstu4145::polynomial{hash});
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

struct curve257: Test
{
    dstu4145::gf2m field{257, 12};

    dstu4145::ecurve curve{
        field,
        0,
        dstu4145::integer{"01CEF494720115657E18F938D7A7942394FF9425C1458C57861F9EEA6ADBE3BE10"}
    };

    dstu4145::integer n{"800000000000000000000000000000006759213AF182E987D3E17714907D470D"};
};

TEST_F(curve257, expand_point_1)
{
    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"2A29EF207D0E9B6C55CD260B306C7E007AC491CA1B10C62334A9E8DCD8D20FB7"},
        dstu4145::integer{"010686D41FF744D4449FCCF6D8EEA03102E6812C93A9D60B978B702CF156D814EF"}
    };

    auto buffer = hex_buffer("2A29EF207D0E9B6C55CD260B306C7E007AC491CA1B10C62334A9E8DCD8D20FB6");
    auto point = curve.expand_point(buffer);

    EXPECT_EQ(point, p);
}

TEST_F(curve257, expand_point_2)
{
    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"01A77131A7C14F9AA6EA8C760D39673D5F0330FAB1118D55B55B7AF0735975485F"},
        dstu4145::integer{"DC058ADA665D99084038B5F914FB9CF7214760A4865B49CAF7F4BE7379F3A395"}
    };

    auto buffer = hex_buffer("01A77131A7C14F9AA6EA8C760D39673D5F0330FAB1118D55B55B7AF0735975485F");
    auto point = curve.expand_point(buffer);

    EXPECT_EQ(point, p);
}

TEST_F(curve257, expand_point_zero)
{
    auto buffer = hex_buffer("00");
    auto point = curve.expand_point(buffer);

    EXPECT_EQ(point, curve.infinity_point());
}

TEST_F(curve257, compress_point_1)
{
    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"2A29EF207D0E9B6C55CD260B306C7E007AC491CA1B10C62334A9E8DCD8D20FB7"},
        dstu4145::integer{"010686D41FF744D4449FCCF6D8EEA03102E6812C93A9D60B978B702CF156D814EF"}
    };
    auto expected = field.create_element(
        dstu4145::polynomial{"2A29EF207D0E9B6C55CD260B306C7E007AC491CA1B10C62334A9E8DCD8D20FB6"}
    );

    auto actual = p.compress();

    EXPECT_EQ(actual, expected);
}

TEST_F(curve257, compress_point_2)
{
    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"01A77131A7C14F9AA6EA8C760D39673D5F0330FAB1118D55B55B7AF0735975485F"},
        dstu4145::integer{"DC058ADA665D99084038B5F914FB9CF7214760A4865B49CAF7F4BE7379F3A395"}
    };
    auto expected = field.create_element(
        dstu4145::polynomial{"01A77131A7C14F9AA6EA8C760D39673D5F0330FAB1118D55B55B7AF0735975485F"}
    );

    auto actual = p.compress();

    EXPECT_EQ(actual, expected);
}

TEST_F(curve257, compress_point_zero)
{
    auto expected = field.create_element(
        dstu4145::polynomial{}
    );
    auto actual = curve.infinity_point().compress();

    EXPECT_EQ(actual, expected);
}

