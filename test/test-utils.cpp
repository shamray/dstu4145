#include <map>

#include "test-utils.h"

namespace
{
    auto decode_char(char c) -> std::byte
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
}

auto hex_buffer(const std::string& hex_string) -> std::vector<std::byte>
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