set(LIB dstu4145)

add_library(${LIB} gf2m.cpp ecurve.cpp gf2m_element.cpp integer.cpp utils.cpp)

#target_include_directories(${LIB})

target_link_libraries(${LIB} boost)