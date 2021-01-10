add_library(test-utils
  test-utils.cpp
  test-utils.h)

target_compile_features(test-utils PUBLIC cxx_std_17)

add_executable(dstu-test
  test-utils-test.cpp
  utils.cpp
  polynomial.cpp
  gf2m.cpp
  integer.cpp
  ecurve.cpp
  dstu.cpp)

target_link_libraries(dstu-test
  test-utils
  dstu4145
  ${CONAN_LIBS})

add_test(NAME DstuTest COMMAND dstu-test)

add_executable(dstu-acceptance-test
  acceptance.cpp)

target_link_libraries(dstu-acceptance-test
  dstu4145
  test-utils
  ${CONAN_LIBS})

add_test(NAME DstuAcceptanceTest COMMAND dstu-acceptance-test)
