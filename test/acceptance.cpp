#include <gtest/gtest.h>

#include "test-utils.h"

#include "signer.h"
#include "verifier.h"

using namespace std::literals;
using namespace testing;

struct acceptance : Test
{
    dstu4145::domain_params params{
        dstu4145::ecurve {
            dstu4145::gf2m {163, 7, 6, 3 },
            1,
            dstu4145::integer{"0x5FF6108462A2DC8210AB403925E638A19C1455D21"},
            dstu4145::integer{"0x400000000000000000002BEC12BE2262D39BCF14D"}
        },
        dstu4145::integer {"0x400000000000000000002BEC12BE2262D39BCF14D"},
        dstu4145::ecurve::point {
            params.curve,
            dstu4145::integer{"0x72D867F93A93AC27DF9FF01AFFE74885C8C540420"},
            dstu4145::integer{"0x0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"}
        }
    };

    dstu4145::rng_t rng {
        [] () {
            static auto buffer = hex_buffer(
                "000000000000000000000001025E40BD97DB012B7A1D79DE8E12932D247F61C6"s
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

TEST_F(acceptance, signing_hash_produces_correct_signature)
{
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto s = dstu4145::signer{params, rng};

    auto signature = s.sign_hash(prv_key, h);
    auto expected = hex_buffer(
        "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_EQ(signature, expected);
}

TEST_F(acceptance, verifying_correct_signature_is_successful)
{
    auto v = dstu4145::verifier{params};
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto signature = hex_buffer(
        "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_TRUE(v.verify_hash(pub_key, h, signature));
}
