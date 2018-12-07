#include <gtest/gtest.h>

#include "test-utils.h"

#include "signer.h"
#include "verifier.h"
#include "rng.h"

#include <random>

using namespace std::literals;
using namespace testing;


struct acceptance163 : Test
{
    dstu4145::ecurve curve {
        dstu4145::gf2m {163, 7, 6, 3 },
        1,
        dstu4145::integer{"5FF6108462A2DC8210AB403925E638A19C1455D21"}
    };

    dstu4145::integer n{"400000000000000000002BEC12BE2262D39BCF14D"};

    dstu4145::rng_t rng{
        [] () {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned char> dis(0);

            return std::byte{ dis(gen) };
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
};

TEST_F(acceptance163, sign_and_verify)
{
    auto fails = 0;
    for (auto i = 0; i < 20; ++i) {

        dstu4145::domain_params params{
            curve,
            n,
            rng
        };

        dstu4145::public_key pub_key{params, prv_key};

        auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
        auto s = dstu4145::signer{prv_key, params, rng};
        auto v = dstu4145::verifier{pub_key, params};

        auto signature = s.sign_hash(h);

        if (!v.verify_hash(h, signature)) {
            ++fails;

            ADD_FAILURE() << std::hex
                          << "p.x= " << params.p.x << std::endl
                          << "p.y= " << params.p.y;
        }
    }

    EXPECT_EQ(0, fails);
}


struct acceptance167 : Test
{
    dstu4145::ecurve curve{
        dstu4145::gf2m {167, 6 },
        1,
        dstu4145::integer{"6EE3CEEB230811759F20518A0930F1A4315A827DAC"}
    };
    dstu4145::integer n {"3FFFFFFFFFFFFFFFFFFFFFB12EBCC7D7F29FF7701F"};

    dstu4145::rng_t rng {
        [] () {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned char> dis(0);

            return std::byte{ dis(gen) };
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
};

TEST_F(acceptance167, sign_and_verify)
{
    auto fails = 0;
    for (auto i = 0; i < 20; ++i) {
        dstu4145::domain_params params {
            curve,
            n,
            rng
        };

        dstu4145::public_key pub_key{params, prv_key};

        auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
        auto s = dstu4145::signer{prv_key, params, rng};
        auto v = dstu4145::verifier{pub_key, params};

        auto signature = s.sign_hash(h);

        if (!v.verify_hash(h, signature)) {
            ++fails;

            ADD_FAILURE() << std::hex
                          << "p.x= " << params.p.x << std::endl
                          << "p.y= " << params.p.y;
        }
    }

    EXPECT_EQ(0, fails);
}


struct acceptance233 : Test
{
    dstu4145::ecurve curve{
        dstu4145::gf2m {233, 9, 4, 1 },
        1,
        dstu4145::integer{"06973B15095675534C7CF7E64A21BD54EF5DD3B8A0326AA936ECE454D2C"}
    };
    dstu4145::integer n{"1000000000000000000000000000013E974E72F8A6922031D2603CFE0D7"};

    dstu4145::rng_t rng {
        [] () {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned char> dis(0);

            return std::byte{ dis(gen) };
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
};



TEST_F(acceptance233, fail_to_find_point)
{
    auto x = dstu4145::integer{"A3FFF2AD7DD48D72D726549C112BB5DCEB725009F4EBC236ECC69EAE0B4F056"};
    auto base_point = curve.find_point(x);

    EXPECT_EQ(base_point, std::nullopt);
}

TEST_F(acceptance233, sign_and_verify)
{
    auto fails = 0;
    for (auto i = 0; i < 20; ++i) {
        dstu4145::domain_params params {
            curve,
            n,
            rng
        };

        dstu4145::public_key pub_key{params, prv_key};

        auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
        auto s = dstu4145::signer{prv_key, params, rng};
        auto v = dstu4145::verifier{pub_key, params};

        auto signature = s.sign_hash(h);

        if (!v.verify_hash(h, signature)) {
            ++fails;

            ADD_FAILURE() << std::hex
                          << "p.x= " << params.p.x << std::endl
                          << "p.y= " << params.p.y;
        }
    }

    EXPECT_EQ(0, fails);
}

struct acceptance257 : Test
{
    dstu4145::ecurve curve{
        dstu4145::gf2m{ 257, 12 },
        0,
        dstu4145::integer{ "1CEF494720115657E18F938D7A7942394FF9425C1458C57861F9EEA6ADBE3BE10" }
    };
    dstu4145::integer n{ "800000000000000000000000000000006759213AF182E987D3E17714907D470D" };

    dstu4145::rng_t rng{
        []() {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned char> dis(0);

            return std::byte{ dis(gen) };
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
};

TEST_F(acceptance257, sign_and_verify)
{
    auto fails = 0;
    for (auto i = 0; i < 10; ++i) {

        dstu4145::domain_params params{
            curve,
            n,
            rng
        };

        dstu4145::public_key pub_key{ params, prv_key };

        auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
        auto s = dstu4145::signer{ prv_key, params, rng };
        auto v = dstu4145::verifier{ pub_key, params };

        auto signature = s.sign_hash(h, 1024);

        if (!v.verify_hash(h, signature)) {
            ++fails;

            ADD_FAILURE() << std::hex
                          << "p.x= " << params.p.x << std::endl
                          << "p.y= " << params.p.y;
        }
    }

    EXPECT_EQ(0, fails);
}

struct acceptance431 : Test
{
    dstu4145::ecurve curve{
        dstu4145::gf2m {431, 5, 3, 1 },
        1,
        dstu4145::integer{"03CE10490F6A708FC26DFE8C3D27C4F94E690134D5BFF988D8D28AAEAEDE975936C66BAC536B18AE2DC312CA493117DAA469C640CAF3"}
    };
    dstu4145::integer n{"3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFBA3175458009A8C0A724F02F81AA8A1FCBAF80D90C7A95110504CF"};

    dstu4145::rng_t rng {
        [] () {
            static std::random_device rd;
            static std::mt19937 gen(rd());
            std::uniform_int_distribution<unsigned char> dis(0);

            return std::byte{ dis(gen) };
        }
    };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
};

TEST_F(acceptance431, sign_and_verify)
{
    auto fails = 0;
    for (auto i = 0; i < 10; ++i) {
        auto base_point = curve.find_point(rng, n);

        dstu4145::domain_params params {
            curve,
            n,
            base_point
        };

        dstu4145::public_key pub_key{params, prv_key};

        auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
        auto s = dstu4145::signer{prv_key, params, rng};
        auto v = dstu4145::verifier{pub_key, params};

        auto signature = s.sign_hash(h, 1024);

        if (!v.verify_hash(h, signature)) {
            ++fails;

            ADD_FAILURE() << std::hex
                          << "p.x= " << params.p.x << std::endl
                          << "p.y= " << params.p.y;
        }
    }

    EXPECT_EQ(0, fails);
}

