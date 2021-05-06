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
    rng_interface.h
    rng.h)

target_include_directories(dstu4145 PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(dstu4145
    ${CONAN_LIBS})

target_compile_features(dstu4145 PUBLIC cxx_std_17)

if(ENABLE_COVERAGE)
    target_compile_options(dstu4145 PRIVATE -O0 --coverage)
    target_link_libraries(dstu4145 --coverage)
endif()

install(TARGETS dstu4145 EXPORT Dstu4145Config
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT Dstu4145Config DESTINATION share/MyLibrary/cmake)

export(TARGETS dstu4145 FILE Dstu4145Config.cmake)