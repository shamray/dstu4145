#include <gtest/gtest.h>

#include "ecurve.h"
#include "domain_params.h"
#include "signer.h"
#include "verifier.h"

using namespace std::literals;
using namespace testing;

TEST(utils, modulo_small)
{
    auto a = dstu4145::integer{"0x63"};
    auto b = dstu4145::integer{"0x2A"};

    EXPECT_EQ(dstu4145::p_modulo(a, b), dstu4145::integer{"0x1D"});

}

TEST(multiply, all)
{
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

TEST(euqlid, simple)
{
    auto [d, a, b] = dstu4145::extended_euqlid(6, 4, 6);
    a; b;

    EXPECT_EQ(d, dstu4145::integer{2});
}

TEST(gf2m, inverse)
{
    auto field = dstu4145::gf2m{7, 5, 2, 1};
    auto element = dstu4145::gf2m_element{field, 42};
    auto inverted = dstu4145::gf2m_element{field, 62};

    EXPECT_EQ(element.inverse(), inverted);
}


struct simple : Test
{
    dstu4145::gf2m field{163, 7, 6, 3 };

    dstu4145::ecurve curve{
        field,
        1,
        dstu4145::integer{"0x5FF6108462A2DC8210AB403925E638A19C1455D21"},
        dstu4145::integer{"0x400000000000000000002BEC12BE2262D39BCF14D"}
    };

    dstu4145::ecurve::point p{
        curve,
        dstu4145::integer{"0x72D867F93A93AC27DF9FF01AFFE74885C8C540420"},
        dstu4145::integer{"0x0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"}
    };

    dstu4145::integer d{"0x183F60FDF7951FF47D67193F8D073790C1C9B5A3E"};
    dstu4145::integer n{"0x400000000000000000002BEC12BE2262D39BCF14D"};
};

TEST_F(simple, point_multiplication)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x57DE7FDE023FF929CB6AC785CE4B79CF64ABDC2DA"},
        dstu4145::integer{"0x695B3B9D26830943133078EF19FE8A8814F8F7B70"}
    };
    auto q = d * p;
    EXPECT_EQ(q, expected);
}

TEST_F(simple, point_addition_equal)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x5A4397672F0C513E390212AD3825420BE83E3BFDA"},
        dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564"}
    };
    auto q = p + p;
    EXPECT_EQ(q, expected);
}

TEST_F(simple, point_addition_not_equal)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x61DA3389DDA324607BFFCC1EAD469465D033EDE07"},
        dstu4145::integer{"0x142B39F71B08662634682D26D3A1F4BE543B393E9"}
    };

    auto r = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x5A4397672F0C513E390212AD3825420BE83E3BFDA"},
        dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564"}
    };
    auto q = r + p;
    EXPECT_EQ(q, expected);
}

TEST_F(simple, field_element_addition_small)
{
    auto a = field.create_element(1);
    auto b = field.create_element(2);

    EXPECT_EQ(a + b, field.create_element(3));
}

TEST_F(simple, field_element_addition_small_equal)
{
    auto a = field.create_element(1);
    auto b = field.create_element(1);

    EXPECT_EQ(a + b, field.create_element(0));
}

TEST_F(simple, field_element_addition_big)
{
    auto a = field.create_element(dstu4145::integer{"0x695B3B9D26830943133078EF19FE8A8814F8F7B70"});
    auto b = field.create_element(dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564"});

    EXPECT_EQ(a + b, field.create_element(dstu4145::integer{"0x5ED757308A030E3F43DC5965A137EA9D61F474E14"}));
}

TEST_F(simple, field_element_multiplication_small)
{
    auto a = field.create_element(2);
    auto b = field.create_element(3);

    EXPECT_EQ(a * b, field.create_element(6));
}

TEST_F(simple, field_element_multiplication_big) {
    auto a = field.create_element(dstu4145::integer{"0x695B3B9D26830943133078EF19FE8A8814F8F7B70"});
    auto b = field.create_element(dstu4145::integer{"0x378C6CADAC80077C50EC218AB8C96015750C83564"});

    EXPECT_EQ(a * b, field.create_element(dstu4145::integer{"0x2F44EF2885428821377088E7AB7110467B10B663B"}));
}

TEST_F(simple, public_key_computation)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x57DE7FDE023FF929CB6AC785CE4B79CF64ABDC2DA"},
        dstu4145::integer{"0x3E85444324BCF06AD85ABF6AD7B5F34770532B9AA"}
    };
    auto q = -(d * p);

    EXPECT_EQ(q, expected);
}

TEST_F(simple, hash_to_field_element)
{
    auto hash = ::dstu4145::integer{"0x09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF" };
    auto h = field.create_element(hash);

    EXPECT_EQ(static_cast<dstu4145::integer>(h), dstu4145::integer{"0x03A2EB95B7180166DDF73532EEB76EDAEF52247FF" });
}

TEST_F(simple, presignature_calculation)
{
    auto expected = dstu4145::ecurve::point{
        curve,
        dstu4145::integer{"0x42A7D756D70E1C9BA62D2CB43707C35204EF3C67C"},
        dstu4145::integer{"0x5310AE5E560464A95DC80286F17EB762EC544B15B"}
    };

    auto e = dstu4145::integer{"0x1025E40BD97DB012B7A1D79DE8E12932D247F61C6"};
    auto ep = e * p;
    EXPECT_EQ(ep, expected);

    auto fe = static_cast<dstu4145::integer>(ep.x);
    EXPECT_EQ(fe, dstu4145::integer{ "0x42A7D756D70E1C9BA62D2CB43707C35204EF3C67C" });
}

TEST_F(simple, signature_calculation)
{
    auto hash = ::dstu4145::integer{ "0x09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF" };

    auto e = dstu4145::integer{ "0x1025E40BD97DB012B7A1D79DE8E12932D247F61C6" };
    auto fe = (e * p).x;

    auto h = field.create_element(hash);
    auto r = static_cast<dstu4145::integer>(h * fe);

    auto dr = static_cast<dstu4145::integer>((static_cast<dstu4145::integer2>(d) * r) % n);
    auto s = (e + dr) % n;

    EXPECT_EQ(r, dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"});
    EXPECT_EQ(s, dstu4145::integer{"0x2100D86957331832B8E8C230F5BD6A332B3615ACA"});
}

TEST_F(simple, signature_verification)
{
    auto q = -(d * p);

    auto hash = ::dstu4145::integer{ "0x09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF" };

    auto h = field.create_element(hash);
    auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto s = dstu4145::integer{"0x2100D86957331832B8E8C230F5BD6A332B3615ACA"};

    auto rpoint = s * p + r * q;

    auto y = h * rpoint.x;

    EXPECT_EQ(static_cast<dstu4145::integer>(y), r);
}

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

    dstu4145::rng_t rng{ [](){ return char{'\x00'}; } };

    dstu4145::private_key prv_key;
    dstu4145::public_key  pub_key;

    static auto decode_char(char c)
    {
        static const auto chars = std::map<char, char> {
            {'0', 0},   {'1', 1},   {'2', 2},   {'3', 3},
            {'4', 4},   {'5', 5},   {'6', 6},   {'7', 7},
            {'8', 8},   {'9', 9},   {'A', 10},  {'B', 11},
            {'C', 12},  {'D', 13},  {'E', 14},  {'F', 15}
        };

        auto found = chars.find(c);
        if (found == std::end(chars))
            throw std::runtime_error("wrong format");

        return found->second;
    }

    static auto hex_buffer(const std::string& hex_string) -> std::vector<unsigned char>
    {
        using namespace std::string_literals;

        if (std::size(hex_string) % 2 != 0)
            return hex_buffer("0"s + hex_string);

        auto result = std::vector<unsigned char>{};
        for (size_t i = 0; i < std::size(hex_string); i += 2) {
            auto c1 = decode_char(hex_string[i]);
            auto c2 = decode_char(hex_string[i+1]);
            result.push_back(c1 * 16 + c2);
        }
        return result;
    }
};

TEST_F(acceptance, hex_buffer)
{
    auto expected = std::vector<unsigned char>{ u'\xBA', u'\xDC', u'\x0D', u'\xE1'};
    EXPECT_EQ(expected, hex_buffer("BADC0DE1"));
}

TEST_F(acceptance, int_to_buffer)
{
    auto r = dstu4145::integer{"0x274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"};
    auto rbuf = std::vector<unsigned char>{};
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


TEST_F(acceptance, sign)
{
    auto rng = [](){
        static auto buffer = hex_buffer(
            "00000000000000000000000183F60FDF7951FF47D67193F8D073790C1C9B5A3E000000000000000000000001025E40BD97DB012B7A1D79DE8E12932D247F61C6"
        );
        static auto current = std::begin(buffer);

        if (current == std::end(buffer))
            return decltype(buffer)::value_type();
        else
            return *current++;
    };
    auto h = hex_buffer("09C9C44277910C9AAEE486883A2EB95B7180166DDF73532EEB76EDAEF52247FF");
    auto s = dstu4145::signer{params, rng};

    auto signature = s.sign_hash(h, prv_key);
    auto expected = hex_buffer(
        "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
        "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
    );

    EXPECT_EQ(signature, expected);
}
