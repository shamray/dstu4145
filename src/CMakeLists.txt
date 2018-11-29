add_library(dstu4145 
    gf2m.cpp
    polynomial.cpp
    polynomial.h
    ecurve.cpp 
    gf2m_element.cpp 
    integer.cpp 
    utils.cpp
    signer.cpp
    signer.h
    verifier.cpp
    verifier.h
    rng_interface.h
    key_pair.cpp
    key_pair.h
    domain_params.cpp
    domain_params.h
    rng.h)

target_include_directories(dstu4145 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(dstu4145
    Boost::boost
    OpenSSL::Crypto)

target_compile_features(dstu4145 PUBLIC cxx_std_17)

install(TARGETS dstu4145 EXPORT Dstu4145Config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT Dstu4145Config DESTINATION share/MyLibrary/cmake)

export(TARGETS dstu4145 FILE Dstu4145Config.cmake)