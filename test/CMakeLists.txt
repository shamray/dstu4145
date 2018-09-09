add_executable(dstu-test
  test-utils.cpp
  test-utils-test.cpp
  utils.cpp
  gf2m.cpp
  integer.cpp
  ecurve.cpp
  dstu.cpp
)

target_link_libraries(dstu-test 
    dstu4145
    Boost::boost 
    GTest::GTest 
    GTest::Main)

add_test(NAME DstuTest COMMAND dstu-test)

add_executable(dstu-acceptance-test
  acceptance.cpp)

target_link_libraries(dstu-acceptance-test
  dstu4145
  Boost::boost
  GTest::GTest
  GTest::Main)

add_test(NAME DstuAcceptanceTest COMMAND dstu-acceptance-test)
