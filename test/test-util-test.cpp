#include <gtest/gtest.h>
#include "test-util.h"

using namespace std::literals;

TEST(test_utils, hex_buffer)
{
    auto expected = std::vector<std::byte>{ std::byte{u'\xBA'}, std::byte{u'\xDC'}, std::byte{u'\x0D'}, std::byte{u'\xE1'}};
    EXPECT_EQ(expected, hex_buffer("BADC0DE1"));
}
