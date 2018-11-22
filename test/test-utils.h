#pragma once

#include <vector>
#include <cstddef>
#include <string>

auto hex_buffer(const std::string& hex_string) -> std::vector<std::byte>;