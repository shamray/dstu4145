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
        dstu4145::ecurve::point {
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
    auto expected = hex_buffer(
        "02100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "0274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_EQ(signature, expected);
}

TEST_F(dstu, compute_presignature)
{
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto engine = dstu4145::engine{ params };

    auto [e, fe] = engine.compute_presignature(rng);

    EXPECT_EQ(e, dstu4145::integer{"01025E40BD97DB012B7A1D79DE8E12932D247F61C6"});
    EXPECT_EQ(fe, curve.field().create_element(dstu4145::integer{"042A7D756D70E1C9BA62D2CB43707C35204EF3C67C"}));
}

TEST_F(dstu, sign_hash_with_presignature)
{
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto engine = dstu4145::engine{ params };

    auto ps = engine.compute_presignature(rng);
    auto signature = engine.sign(ps, prv_key, h);
    auto expected = hex_buffer(
        "02100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "0274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
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