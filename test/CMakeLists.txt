add_executable(dstu-test
  main.cpp
  acceptance.cpp
  integer.cpp
  test-util.cpp
  test-util-test.cpp
)

target_link_libraries(dstu-test 
    dstu4145
    Boost::boost 
    GTest::GTest 
    GTest::Main)

add_test(NAME DstuTest COMMAND dstu-test)

 
