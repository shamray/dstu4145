find_package(OpenSSL REQUIRED)
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(dstu-jks-tests
  jks.cpp
  jks.h
  jks_test.cpp)

target_link_libraries(dstu-jks-tests PRIVATE
  test-utils
  dstu4145::dstu4145
  OpenSSL::Crypto
  GTest::gtest
  GTest::gtest_main)

target_compile_definitions(dstu-jks-tests PRIVATE
  JKS_FIXTURE="${CMAKE_CURRENT_SOURCE_DIR}/testdata/store.jks")

gtest_discover_tests(
  dstu-jks-tests
)
