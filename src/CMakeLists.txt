include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${GTEST_INCLUDE_DIRS} )

message(${GTEST_INCLUDE_DIRS})

add_executable(dstu-test main.cpp gf2m.cpp)
target_link_libraries(dstu-test ${GTEST_BOTH_LIBRARIES})

add_test(NAME DstuTest COMMAND dstu-test)