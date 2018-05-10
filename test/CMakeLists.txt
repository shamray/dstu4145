add_executable(dstu-test
  main.cpp
  acceptance.cpp
  integer.cpp
  utils.cpp
  test-utils.cpp
  test-utils-test.cpp
)

target_link_libraries(dstu-test 
    dstu4145
    Boost::boost 
    GTest::GTest 
    GTest::Main)

add_test(NAME DstuTest COMMAND dstu-test)

 
