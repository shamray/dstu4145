#include <gtest/gtest.h>

#include "test-utils.h"

#include "engine.h"
#include "rng.h"

#include <random>

using namespace std::literals;
using namespace testing;

// Test case from DSTU 4145-2002 Appendix B
struct dstu : Test
{
    dstu4145::ecurve curve {
        dstu4145::gf2m {163, 7, 6, 3 },
        1,
        dstu4145::integer{"5FF6108462A2DC8210AB403925E638A19C1455D21"}
    };
    dstu4145::domain_params params {
        curve,
        dstu4145::integer{"400000000000000000002BEC12BE2262D39BCF14D"},
        dstu4145::ecurve_point {
            curve,
            dstu4145::integer{"72D867F93A93AC27DF9FF01AFFE74885C8C540420"},
            dstu4145::integer{"0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"}
        }
    };

    dstu4145::rng_t rng {
        [] () {
            static auto buffer = hex_buffer(
                "01025E40BD97DB012B7A1D79DE8E12932D247F61C6"s
            );
            static auto current = std::begin(buffer);

            if (current == std::end(buffer))
                return decltype(buffer)::value_type();
            else
                return *current++;
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
    dstu4145::public_key  pub_key{params, prv_key};
};

TEST_F(dstu, signing_hash_produces_correct_signature)
{
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto engine = dstu4145::engine{params};

    auto signature = engine.sign(rng, prv_key, h);

    auto expectedL = hex_buffer("02100D86957331832B8E8C230F5BD6A332B3615ACA");
    auto expectedR = hex_buffer("0274EA2C0CAA014A0D80A424F59ADE7A93068D08A7");

    ASSERT_GE(signature.size(), expectedL.size() + expectedR.size());

    std::vector<std::byte> expected(signature.size(), std::byte{ 0 });
    std::copy(
        expectedL.begin(), expectedL.end(),
        expected.begin() + (expected.size() / 2 - expectedL.size())
    );
    std::copy(
        expectedR.begin(), expectedR.end(),
        expected.begin() + (expected.size() - expectedR.size())
    );

    EXPECT_EQ(signature, expected);
}

TEST_F(dstu, compute_presignature)
{
    auto engine = dstu4145::engine{ params };

    auto [e, fe] = engine.compute_presignature(rng);

    EXPECT_EQ(e, dstu4145::integer{"01025E40BD97DB012B7A1D79DE8E12932D247F61C6"});
    EXPECT_EQ(fe, curve.field().element(dstu4145::integer{"042A7D756D70E1C9BA62D2CB43707C35204EF3C67C"}));
}

TEST_F(dstu, sign_hash_with_presignature)
{
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto engine = dstu4145::engine{ params };

    auto ps = engine.compute_presignature(rng);
    auto signature = engine.sign(ps, prv_key, h);

    auto expectedL = hex_buffer("02100D86957331832B8E8C230F5BD6A332B3615ACA");
    auto expectedR = hex_buffer("0274EA2C0CAA014A0D80A424F59ADE7A93068D08A7");

    ASSERT_GE(signature.size(), expectedL.size() + expectedR.size());

    std::vector<std::byte> expected(signature.size(), std::byte{0});
    std::copy(
        expectedL.begin(), expectedL.end(),
        expected.begin() + (expected.size() / 2 - expectedL.size())
    );
    std::copy(
        expectedR.begin(), expectedR.end(),
        expected.begin() + (expected.size() - expectedR.size())
    );

    EXPECT_EQ(signature, expected);
}

TEST_F(dstu, verifying_correct_signature_is_successful)
{
    auto engine = dstu4145::engine{params};
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto signature = hex_buffer(
        "02100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "0274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_TRUE(engine.verify(pub_key, h, signature));
}

TEST_F(dstu, verifying_correct_signature_is_successful_longer_signature)
{
    auto engine = dstu4145::engine{params};
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto signature = hex_buffer(
        "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_TRUE(engine.verify(pub_key, h, signature));
}

TEST_F(dstu, verifying_malformed_signature_returns_false)
{
    auto engine = dstu4145::engine{params};
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");

    auto zero = std::string(64, '0');
    auto s = "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s;
    auto r = "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s;

    EXPECT_FALSE(engine.verify(pub_key, h, {}));
    EXPECT_FALSE(engine.verify(pub_key, h, hex_buffer("0011223344556677889900"s)));
    EXPECT_FALSE(engine.verify(pub_key, h, hex_buffer(zero + r)));
    EXPECT_FALSE(engine.verify(pub_key, h, hex_buffer(s + zero)));
    EXPECT_FALSE(engine.verify(pub_key, h, hex_buffer(zero + zero)));
}

TEST_F(dstu, verifying_rejects_signature_with_s_shifted_by_n)
{
    auto engine = dstu4145::engine{params};
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");

    // s + n verifies under the raw equation, so without the s < n check it is a
    // second valid signature for the same message.
    auto signature = hex_buffer(
        "000000000000000000000006100D86957331832B8E8EE1D087B8C95FED1E4C17"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_FALSE(engine.verify(pub_key, h, signature));
}

TEST_F(dstu, signing_zero_hash_is_possible)
{
    auto engine = dstu4145::engine{params};
    auto h = dstu4145::buffer(32, std::byte{0});

    auto signature = engine.sign(rng, prv_key, h);

    EXPECT_TRUE(engine.verify(pub_key, h, signature));
}

TEST_F(dstu, private_key_serialization)
{
    std::vector<std::byte> buffer;
    prv_key.to_buffer(std::back_inserter(buffer));

    auto deserialized = dstu4145::private_key{buffer};

    EXPECT_EQ(prv_key, deserialized);
}

TEST_F(dstu, public_key_serialization)
{
    std::vector<std::byte> buffer;
    pub_key.to_buffer(std::back_inserter(buffer));

    auto deserialized = dstu4145::public_key{params, buffer};

    EXPECT_EQ(pub_key, deserialized);
}

struct dstu257 : Test
{
    dstu4145::ecurve curve {
        dstu4145::gf2m {257, 12},
        0,
        dstu4145::integer{"01CEF494720115657E18F938D7A7942394FF9425C1458C57861F9EEA6ADBE3BE10"}
    };
    dstu4145::domain_params params {
        curve,
        dstu4145::integer{"800000000000000000000000000000006759213AF182E987D3E17714907D470D"},
        dstu4145::ecurve_point {
            curve,
            dstu4145::integer{"2A29EF207D0E9B6C55CD260B306C7E007AC491CA1B10C62334A9E8DCD8D20FB7"},
            dstu4145::integer{"010686D41FF744D4449FCCF6D8EEA03102E6812C93A9D60B978B702CF156D814EF"}
        }
    };
};

TEST_F(dstu257, public_key_rejects_point_at_infinity)
{
    auto zero = dstu4145::buffer(32, std::byte{0});

    EXPECT_THROW((dstu4145::public_key{params, zero}), std::runtime_error);
}

TEST_F(dstu257, public_key_rejects_value_not_on_curve)
{
    EXPECT_THROW((dstu4145::public_key{params, hex_buffer("02"s)}), std::runtime_error);
}

TEST_F(dstu257, public_key_rejects_point_of_wrong_order)
{
    // (0, sqrt(b)) is the point of order 2, so p + t has order 2n but still
    // survives compression on a curve with cofactor 4.
    auto sqrt_b = curve.b();
    for (auto i = 1; i < 257; ++i)
        sqrt_b = square(sqrt_b);
    auto t = dstu4145::ecurve_point{curve, curve.field().element(0), sqrt_b};

    auto buffer = dstu4145::buffer{};
    (params.p + t).compress().to_buffer(std::back_inserter(buffer));

    EXPECT_THROW((dstu4145::public_key{params, buffer}), std::runtime_error);
}

TEST_F(dstu257, domain_params_reject_point_at_infinity)
{
    EXPECT_THROW((dstu4145::domain_params{curve, params.n, curve.infinity_point()}), std::runtime_error);
}

TEST_F(dstu257, domain_params_reject_point_not_on_curve)
{
    auto p = params.p;
    p.y = p.y + curve.field().element(1);

    EXPECT_THROW((dstu4145::domain_params{curve, params.n, p}), std::runtime_error);
}

TEST_F(dstu257, domain_params_reject_wrong_order)
{
    auto n_minus_1 = dstu4145::integer{"800000000000000000000000000000006759213AF182E987D3E17714907D470C"};

    EXPECT_THROW((dstu4145::domain_params{curve, n_minus_1, params.p}), std::runtime_error);
}

TEST(key_pair, private_key_constructor)
{
    auto b = hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s);
    dstu4145::private_key pk1{b};
    dstu4145::private_key pk2{dstu4145::integer(b)};

    EXPECT_EQ(pk1, pk2);
}