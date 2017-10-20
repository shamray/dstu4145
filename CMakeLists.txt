cmake_minimum_required(VERSION 3.0)

project(dstu4145)

set (GTEST_MSVC_SEARCH "MT")
set (GTEST_ROOT "C:\\libdev\\googlemock_googletest\\googletest")
set (BOOST_ROOT "C:\\libdev\\boost")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

find_package(Boost 1.64 REQUIRED)
add_library(boost INTERFACE IMPORTED)
set_property(TARGET boost PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

find_package(GTest REQUIRED)
add_library(googletest STATIC IMPORTED)
set_property(TARGET googletest
        PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${GTEST_INCLUDE_DIRS}
        PROPERTY IMPORTED_LOCATION ${GTEST_BOTH_LIBRARIES})

enable_testing()
 
add_subdirectory(src)
add_subdirectory(test)