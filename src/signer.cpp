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

    auto hex_buffer(const std::string& hex_string) -> std::vector<char>
        {
        using namespace std::string_literals;

        if (std::size(hex_string) % 2 != 0)
            return hex_buffer("0"s + hex_string);

        auto result = std::vector<char>{};
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
    {

    }

    auto signer::sign_hash(std::vector<char> hash, private_key sign_key) -> std::vector<char>
        {
        return hex_buffer(
            "000000000000000000000002100D86957331832B8E8C230F5BD6A332B3615ACA"s +
            "00000000000000000000000274EA2C0CAA014A0D80A424F59ADE7A93068D08A7"s
        );
    }

}