cmake_minimum_required(VERSION 3.0)

set (GTEST_MSVC_SEARCH "MT")
set (GTEST_ROOT "C:\\libdev\\googlemock_googletest\\googletest")
set (BOOST_ROOT "C:\\libdev\\boost")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

find_package(Boost 1.64 REQUIRED)
find_package(GTest REQUIRED)

enable_testing()
 
add_subdirectory(src)