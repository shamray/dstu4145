#include <gtest/gtest.h>

#include "test-utils.h"

#include "signer.h"
#include "verifier.h"
#include "rng.h"

#include <random>

using namespace std::literals;
using namespace testing;

struct acceptance : Test
{
    dstu4145::ecurve curve {
        dstu4145::gf2m {163, 7, 6, 3 },
        1,
        dstu4145::integer{"0x5FF6108462A2DC8210AB403925E638A19C1455D21"}
    };
    dstu4145::domain_params params {
        curve,
        dstu4145::integer {"0x400000000000000000002BEC12BE2262D39BCF14D"},
        dstu4145::ecurve::point {
            curve,
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
    auto s = dstu4145::signer{prv_key, params, rng};

    auto signature = s.sign_hash(h);
    auto expected = hex_buffer(
        "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_EQ(signature, expected);
}

TEST_F(acceptance, verifying_correct_signature_is_successful)
{
    auto v = dstu4145::verifier{pub_key, params};
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto signature = hex_buffer(
        "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_TRUE(v.verify_hash(h, signature));
}

struct acceptance233 : Test
{
    dstu4145::ecurve curve{
        dstu4145::gf2m {233, 9, 4, 1 },
        1,
        dstu4145::integer{"0x06973B15095675534C7CF7E64A21BD54EF5DD3B8A0326AA936ECE454D2C"}
    };
    dstu4145::integer n {"0x1000000000000000000000000000013E974E72F8A6922031D2603CFE0D7"};

    dstu4145::rng_t rng {
        [] () {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned char> dis(0);

            auto x = dis(gen);
            std::cerr << std::hex << int(x);

            return std::byte{ x };
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
};

TEST_F(acceptance233, fail_to_find_point)
{
    auto x = dstu4145::integer{"0xA3FFF2AD7DD48D72D726549C112BB5DCEB725009F4EBC236ECC69EAE0B4F056"};
    auto base_point = curve.find_point(x);

    EXPECT_EQ(base_point, std::nullopt);
}

TEST_F(acceptance233, wtf)
{
    dstu4145::domain_params params {
        curve,
        n,
        dstu4145::ecurve::point {
            curve,
            dstu4145::integer{"0x17F07BFE343D04769E1F4A6DFA802E59AE878EEB53968DAD36BBDDFBD85"},
            dstu4145::integer{"0x1D35F0F428DE1F301B8D21176937B1DC9E255977A3D11EA985707F7B3B8"}
        }
    };

    dstu4145::public_key pub_key{params, prv_key};

    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto s = dstu4145::signer{prv_key, params, rng};
    auto v = dstu4145::verifier{pub_key, params};

    auto signature = s.sign_hash(h);

    EXPECT_TRUE(v.verify_hash(h, signature));
}

TEST_F(acceptance233, sign_and_verify)
{
    auto base_point = curve.find_point(rng);

    dstu4145::domain_params params {
        curve,
        n,
        base_point
    };

    std::cerr << std::endl;
    std::cerr << "p.x = " << base_point.x << std::endl;
    std::cerr << "p.y = " << base_point.y << std::endl;

    dstu4145::public_key pub_key{params, prv_key};

    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto s = dstu4145::signer{prv_key, params, rng};
    auto v = dstu4145::verifier{pub_key, params};

    auto signature = s.sign_hash(h);

    EXPECT_TRUE(v.verify_hash(h, signature));
}