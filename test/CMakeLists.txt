find_package(GTest REQUIRED)
include(GoogleTest)

add_library(test-utils
  test-utils.cpp
  test-utils.h)

target_compile_features(test-utils PUBLIC cxx_std_17)

add_executable(dstu-unit-tests
  test-utils-test.cpp
  arithmetic.cpp
  polynomial.cpp
  gf2m.cpp
  integer.cpp
  ecurve.cpp
  dstu.cpp)

target_link_libraries(dstu-unit-tests PRIVATE
  test-utils
  dstu4145::dstu4145
  GTest::gtest
  GTest::gtest_main)

if(ENABLE_COVERAGE)
  target_compile_options(dstu-unit-tests PRIVATE -O0 -fno-inline -fno-inline-small-functions -fno-default-inline)
  target_link_libraries(dstu-unit-tests PRIVATE --coverage)
endif()

gtest_discover_tests(
  dstu-unit-tests
)

add_executable(dstu-acceptance-test
  acceptance.cpp)

target_link_libraries(dstu-acceptance-test PRIVATE
  dstu4145::dstu4145
  test-utils
  GTest::gtest
  GTest::gtest_main)

gtest_discover_tests(
  dstu-acceptance-test
  PROPERTIES LABELS acceptance
)
