#include "signer.h"

#include <map>

using namespace std::string_literals;

namespace
{
    auto decode_char(char c)
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

    auto hex_buffer(const std::string& hex_string) -> std::vector<unsigned char>
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
}

namespace dstu4145
{
    signer::signer(domain_params params, rng_t random)
        : params_(params)
        , random_(random)
    {

    }

    auto signer::sign_hash(std::vector<unsigned char> hash, private_key sign_key) -> std::vector<unsigned char>
    {
        gf2m field{163, 7, 6, 3 };
        ecurve curve{
            field,
            1,
            integer{"0x5FF6108462A2DC8210AB403925E638A19C1455D21"},
            integer{"0x400000000000000000002BEC12BE2262D39BCF14D"}
        };
        ecurve::point p{
            curve,
            integer{"0x72D867F93A93AC27DF9FF01AFFE74885C8C540420"},
            integer{"0x0224A9C3947852B97C5599D5F4AB81122ADC3FD9B"}
        };
        integer d{"0x183F60FDF7951FF47D67193F8D073790C1C9B5A3E"};
        integer n{"0x400000000000000000002BEC12BE2262D39BCF14D"};

        auto e = integer{"0x1025E40BD97DB012B7A1D79DE8E12932D247F61C6"};
        auto fe = (e * p).x;

        auto h = field.create_element(buffer_to_integer(hash));
        auto r = static_cast<integer>(h * fe);

        auto dr = static_cast<integer>((static_cast<integer2>(d) * r) % n);
        auto s = (e + dr) % n;

        auto result = std::vector<unsigned char>{};

        integer_to_buffer(s, std::back_inserter(result));
        integer_to_buffer(r, std::back_inserter(result));

        return result;

    }

}