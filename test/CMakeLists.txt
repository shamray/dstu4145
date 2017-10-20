add_executable(dstu-test main.cpp)

get_property(XXX TARGET googletest PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
message(${XXX})
target_link_libraries(dstu-test boost googletest)

add_test(NAME DstuTest COMMAND dstu-test)