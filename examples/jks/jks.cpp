#include "jks.h"

#include <openssl/crypto.h>
#include <openssl/evp.h>
#include <openssl/objects.h>
#include <openssl/x509.h>

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>

namespace dstu4145::jks
{
    namespace
    {
        const auto magic = std::uint64_t{0xFEEDFEED};
        const auto version = std::uint64_t{2};
        const auto digest_size = std::size_t{20};
        const auto key_protector_oid = "1.3.6.1.4.1.42.2.17.1.1";

        // Big-endian reader that never reads past the end of the data.
        class reader
        {
        public:
            explicit reader(const buffer& data)
                : data_{data}
            {
            }

            auto bytes(std::size_t count) -> const std::byte*
            {
                if (count > data_.size() - pos_)
                    throw error{errc::truncated, "truncated keystore"};

                auto result = data_.data() + pos_;
                pos_ += count;
                return result;
            }

            auto number(std::size_t size) -> std::uint64_t
            {
                auto p = bytes(size);
                auto result = std::uint64_t{0};
                for (auto i = std::size_t{0}; i < size; ++i)
                    result = result << 8 | std::to_integer<std::uint64_t>(p[i]);

                return result;
            }

            auto string() -> std::string
            {
                auto size = number(2);
                auto p = reinterpret_cast<const char*>(bytes(size));
                return {p, size};
            }

            auto blob() -> buffer
            {
                auto size = number(4);
                auto p = bytes(size);
                return {p, p + size};
            }

            // Certificate type followed by the encoded certificate.
            auto certificate() -> buffer
            {
                string();
                return blob();
            }

            auto remaining() const -> std::size_t
            {
                return data_.size() - pos_;
            }

        private:
            const buffer& data_;
            std::size_t pos_ = 0;
        };

        auto invalid_utf8() -> std::invalid_argument
        {
            return std::invalid_argument{"password is not valid UTF-8"};
        }

        auto next_code_point(std::string_view utf8, std::size_t& pos) -> char32_t
        {
            auto lead = static_cast<unsigned char>(utf8[pos++]);
            if (lead < 0x80)
                return lead;

            // 0x80-0xC1 are continuation bytes or overlong leads, 0xF5-0xFF can't
            // encode anything up to U+10FFFF.
            if (lead < 0xC2 || lead > 0xF4)
                throw invalid_utf8();

            auto length = lead >= 0xF0 ? 3u : lead >= 0xE0 ? 2u : 1u;
            if (length > utf8.size() - pos)
                throw invalid_utf8();

            auto result = char32_t{lead & (0x3Fu >> length)};
            for (auto i = 0u; i < length; ++i) {
                auto c = static_cast<unsigned char>(utf8[pos++]);
                if ((c & 0xC0) != 0x80)
                    throw invalid_utf8();

                result = result << 6 | (c & 0x3F);
            }

            static const char32_t shortest[] = {0, 0x80, 0x800, 0x10000};
            if (result < shortest[length] || result > 0x10FFFF || (result >= 0xD800 && result <= 0xDFFF))
                throw invalid_utf8();

            return result;
        }
    }

    error::error(errc code, const char* what)
        : std::runtime_error{what}
        , code{code}
    {
    }

    auto load(const buffer& file, std::string_view password) -> std::vector<entry>
    {
        auto in = reader{file};

        if (in.number(4) != magic)
            throw error{errc::bad_magic, "not a JKS keystore"};

        if (in.number(4) != version)
            throw error{errc::bad_version, "unsupported JKS version"};

        auto entries = std::vector<entry>{};
        for (auto count = in.number(4); count > 0; --count) {
            auto tag = in.number(4);
            if (tag != 1 && tag != 2)
                throw error{errc::malformed, "unknown keystore entry type"};

            auto e = entry{};
            e.alias = in.string();
            in.number(8); // timestamp

            if (tag == 1) {
                e.encrypted_key = in.blob();
                for (auto certs = in.number(4); certs > 0; --certs)
                    e.certificates.push_back(in.certificate());
            } else {
                e.certificates.push_back(in.certificate());
            }

            entries.push_back(std::move(e));
        }

        auto expected = in.bytes(digest_size);
        if (in.remaining() != 0)
            throw error{errc::malformed, "unexpected data after keystore digest"};

        auto pw = detail::utf16be(password);
        auto digest = detail::sha1({
            {pw.data(), pw.size()},
            {"Mighty Aphrodite", 16},
            {file.data(), file.size() - digest_size}
        });

        if (CRYPTO_memcmp(digest.data(), expected, digest_size) != 0)
            throw error{errc::bad_store_password, "wrong store password or corrupted keystore"};

        return entries;
    }

    // Keystream blocks are SHA1(password || previous block), starting from the
    // salt; the last 20 bytes are SHA1(password || plaintext).
    auto decrypt_key(const buffer& encrypted_key, std::string_view password) -> buffer
    {
        const auto malformed = [] { return error{errc::malformed, "malformed protected key"}; };

        if (encrypted_key.size() > static_cast<std::size_t>(std::numeric_limits<long>::max()))
            throw malformed();

        auto begin = reinterpret_cast<const unsigned char*>(encrypted_key.data());
        auto p = begin;
        auto info = std::unique_ptr<X509_SIG, decltype(&X509_SIG_free)>{
            d2i_X509_SIG(nullptr, &p, static_cast<long>(encrypted_key.size())),
            X509_SIG_free
        };
        if (!info || p != begin + encrypted_key.size())
            throw malformed();

        const X509_ALGOR* algorithm = nullptr;
        const ASN1_OCTET_STRING* payload = nullptr;
        X509_SIG_get0(info.get(), &algorithm, &payload);

        const ASN1_OBJECT* oid = nullptr;
        X509_ALGOR_get0(&oid, nullptr, nullptr, algorithm);

        char oid_text[64];
        if (OBJ_obj2txt(oid_text, sizeof oid_text, oid, 1) <= 0 || std::strcmp(oid_text, key_protector_oid) != 0)
            throw malformed();

        // salt, at least one byte of ciphertext, digest
        const auto size = static_cast<std::size_t>(ASN1_STRING_length(payload));
        if (size <= 2 * digest_size)
            throw malformed();

        const auto data = reinterpret_cast<const std::byte*>(ASN1_STRING_get0_data(payload));
        const auto ciphertext = data + digest_size;
        const auto expected = data + size - digest_size;

        auto pw = detail::utf16be(password);
        auto key = buffer(data, data + digest_size);
        auto plaintext = buffer(size - 2 * digest_size);

        for (auto i = std::size_t{0}; i < plaintext.size(); i += digest_size) {
            auto block = detail::sha1({{pw.data(), pw.size()}, {key.data(), key.size()}});
            std::copy(std::begin(block), std::end(block), std::begin(key));

            for (auto j = i; j < std::min(i + digest_size, plaintext.size()); ++j)
                plaintext[j] = ciphertext[j] ^ key[j - i];
        }

        auto digest = detail::sha1({{pw.data(), pw.size()}, {plaintext.data(), plaintext.size()}});
        if (CRYPTO_memcmp(digest.data(), expected, digest_size) != 0)
            throw error{errc::bad_key_password, "wrong key password or corrupted key"};

        return plaintext;
    }

    namespace detail
    {
        auto sha1(std::initializer_list<byte_range> parts) -> std::array<std::byte, 20>
        {
            auto ctx = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>{EVP_MD_CTX_new(), EVP_MD_CTX_free};

            auto ok = ctx && EVP_DigestInit_ex(ctx.get(), EVP_sha1(), nullptr);
            for (const auto& [data, size] : parts)
                ok = ok && EVP_DigestUpdate(ctx.get(), data, size);

            auto result = std::array<std::byte, 20>{};
            ok = ok && EVP_DigestFinal_ex(ctx.get(), reinterpret_cast<unsigned char*>(result.data()), nullptr);
            if (!ok)
                throw std::runtime_error("SHA-1 computation failed");

            return result;
        }

        auto utf16be(std::string_view utf8) -> buffer
        {
            // Two passes so the result is allocated once at its final size.
            auto units = std::size_t{0};
            for (auto pos = std::size_t{0}; pos < utf8.size();)
                units += next_code_point(utf8, pos) > 0xFFFF ? 2 : 1;

            auto result = buffer(units * 2);
            auto out = std::begin(result);
            const auto put = [&out] (char32_t unit) {
                *out++ = std::byte(unit >> 8);
                *out++ = std::byte(unit & 0xFF);
            };

            for (auto pos = std::size_t{0}; pos < utf8.size();) {
                auto c = next_code_point(utf8, pos);
                if (c > 0xFFFF) {
                    c -= 0x10000;
                    put(0xD800 + (c >> 10));
                    put(0xDC00 + (c & 0x3FF));
                } else {
                    put(c);
                }
            }

            return result;
        }
    }
}
