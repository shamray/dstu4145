find_package(OpenSSL REQUIRED)

add_library(dstu4145
    is_container.h
    gf2m.cpp
    gf2m.h
    polynomial.cpp
    polynomial.h
    ecurve.cpp
    ecurve.h
    gf2m_element.cpp
    gf2m_element.h
    integer.cpp
    integer.h
    arithmetic.cpp
    arithmetic.h
    engine.cpp
    engine.h
    rng_interface.h
    key_pair.cpp
    key_pair.h
    domain_params.cpp
    domain_params.h
    rng.h)
add_library(dstu4145::dstu4145 ALIAS dstu4145)

target_sources(dstu4145 PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
    FILES
        arithmetic.h
        domain_params.h
        ecurve.h
        engine.h
        gf2m.h
        gf2m_element.h
        integer.h
        is_container.h
        key_pair.h
        polynomial.h
        rng.h
        rng_interface.h)

target_link_libraries(dstu4145 PUBLIC OpenSSL::Crypto)

target_compile_features(dstu4145 PUBLIC cxx_std_17)

if(ENABLE_COVERAGE)
    target_compile_options(dstu4145 PRIVATE -O0 --coverage -fno-inline -fno-inline-small-functions -fno-default-inline)
    target_link_libraries(dstu4145 PRIVATE --coverage)
endif()

include(CMakePackageConfigHelpers)

install(TARGETS dstu4145
    EXPORT dstu4145Targets
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT dstu4145Targets
    FILE dstu4145Targets.cmake
    NAMESPACE dstu4145::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dstu4145)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/dstu4145Config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/dstu4145Config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dstu4145)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/dstu4145ConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/dstu4145Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/dstu4145ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/dstu4145)

export(EXPORT dstu4145Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/dstu4145Targets.cmake
    NAMESPACE dstu4145::)
