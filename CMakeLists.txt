cmake_minimum_required(VERSION 3.0)
project(dstu4145 LANGUAGES CXX)

include(GNUInstallDirs)

set (GTEST_MSVC_SEARCH "MT")
find_package(Boost 1.62 REQUIRED QUIET)
find_package(GTest REQUIRED QUIET)

enable_testing()
add_subdirectory(src)
add_subdirectory(test)


