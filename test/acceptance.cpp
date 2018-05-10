#include <gtest/gtest.h>

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

    dstu4145::rng_t rng{ [](){ return std::byte{'\x00'}; } };

    dstu4145::private_key prv_key{hex_buffer("00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E"s)};
    dstu4145::public_key  pub_key{params, prv_key};

    static auto decode_char(char c)
    {
        static const auto chars = std::map<char, unsigned char> {
            {'0', 0},   {'1', 1},   {'2', 2},   {'3', 3},
            {'4', 4},   {'5', 5},   {'6', 6},   {'7', 7},
            {'8', 8},   {'9', 9},   {'A', 10},  {'B', 11},
            {'C', 12},  {'D', 13},  {'E', 14},  {'F', 15}
        };

        auto found = chars.find(c);
        if (found == std::end(chars))
            throw std::runtime_error("wrong format");

        return std::byte{ found->second };
    }

    static auto hex_buffer(const std::string& hex_string) -> std::vector<std::byte>
    {
        using namespace std::string_literals;

        if (std::size(hex_string) % 2 != 0)
            return hex_buffer("0"s + hex_string);

        auto result = std::vector<std::byte>{};
        for (size_t i = 0; i < std::size(hex_string); i += 2) {
            auto c1 = decode_char(hex_string[i]);
            auto c2 = decode_char(hex_string[i+1]);
            result.push_back(std::byte{(c1 << 4) | c2});
        }
        return result;
    }
};

TEST_F(acceptance, hex_buffer)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\xBA'}, std::byte{u'\xDC'}, std::byte{u'\x0D'}, std::byte{u'\xE1'}};
    EXPECT_EQ(expected, hex_buffer("BADC0DE1"));
}

TEST_F(acceptance, int_to_buffer)
{
    auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto rbuf = std::vector<std::byte>{};
    auto expected = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    dstu4145::integer_to_buffer(r, std::back_inserter(rbuf));

    EXPECT_EQ(rbuf, expected);
}

TEST_F(acceptance, buffer_to_int)
{
    auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto rbuf = hex_buffer("00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s);

    EXPECT_EQ(r, dstu4145::buffer_to_integer(rbuf));
}


TEST_F(acceptance, signing_hash_produces_correct_signature)
{
    auto rng = [](){
        static auto buffer = hex_buffer(
            "000000000000000000000001025E40BD97DB012B7A1D79DE8E12932D247F61C6"s
        );
        static auto current = std::begin(buffer);

        if (current == std::end(buffer))
            return decltype(buffer)::value_type();
        else
            return *current++;
    };
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
