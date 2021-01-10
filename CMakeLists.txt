cmake_minimum_required(VERSION 3.0)
project(dstu4145 LANGUAGES CXX)

include(GNUInstallDirs)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.16.1/conan.cmake"
        "${CMAKE_BINARY_DIR}/conan.cmake"
        EXPECTED_HASH SHA256=396e16d0f5eabdc6a14afddbcfff62a54a7ee75c6da23f32f7a31bc85db23484
        TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt  # or relative build/conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD missing)

conan_load_buildinfo()
conan_basic_setup()

enable_testing()
add_subdirectory(src)
add_subdirectory(test)


