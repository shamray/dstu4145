#pragma once

#include "engine.h"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Java KeyStore (JKS) reader and decryption of keys
namespace dstu4145::jks
{
    enum class errc
    {
        bad_magic,
        bad_version,
        truncated,
        malformed,
        bad_store_password,
        bad_key_password
    };

    class error : public std::runtime_error
    {
    public:
        error(errc code, const char *what);

        errc code;
    };

    struct entry
    {
        std::string alias;
        buffer encrypted_key; // empty for trusted certificate entries
        std::vector<buffer> certificates;
    };

    // Passwords are UTF-8; std::invalid_argument is thrown if they are not.
    auto load(const buffer &file, std::string_view password) -> std::vector<entry>;
    auto decrypt_key(const buffer &encrypted_key, std::string_view password) -> buffer;

    namespace detail
    {
        using byte_range = std::pair<const void *, std::size_t>;

        auto sha1(std::initializer_list<byte_range> parts) -> std::array<std::byte, 20>;
        auto utf16be(std::string_view utf8) -> buffer;
    }
}
