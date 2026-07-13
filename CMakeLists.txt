cmake_minimum_required(VERSION 3.23)
project(dstu4145 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
    option(ENABLE_COVERAGE "Enable code coverage for gcc/clang" FALSE)
endif()

include(GNUInstallDirs)
include(CTest)

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()
