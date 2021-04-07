#include <gtest/gtest.h>
#include "test-utils.h"

using namespace std::literals;

TEST(test_utils, hex_buffer)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\xBA'}, std::byte{u'\xDC'}, std::byte{u'\x0D'}, std::byte{u'\xE1'}};
    EXPECT_EQ(expected, hex_buffer("BADC0DE1"));
}

TEST(test_utils, hex_buffer_zero)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\x00'} };
    EXPECT_EQ(expected, hex_buffer("00"));
}

TEST(test_utils, hex_buffer_one)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\x01'} };
    EXPECT_EQ(expected, hex_buffer("1"));
}

TEST(test_utils, hex_buffer_one_padded)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\x00'} , std::byte{u'\x01'} };
    EXPECT_EQ(expected, hex_buffer("0001"));
}

TEST(test_utils, hex_buffer_one_padded_odd)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\x00'} , std::byte{u'\x01'} };
    EXPECT_EQ(expected, hex_buffer("001"));
}
