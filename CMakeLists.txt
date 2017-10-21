cmake_minimum_required(VERSION 3.0)

project(dstu4145)
enable_testing()

set (GTEST_MSVC_SEARCH "MT")

find_package(Boost 1.64 REQUIRED QUIET)
find_package(GTest REQUIRED QUIET)

add_subdirectory(src)
add_subdirectory(test)


