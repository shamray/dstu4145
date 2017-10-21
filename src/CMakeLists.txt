add_library(dstu4145 
    gf2m.cpp 
    ecurve.cpp 
    gf2m_element.cpp 
    integer.cpp 
    utils.cpp signer.cpp signer.h verifier.cpp verifier.h rng_interface.h key_pair.cpp key_pair.h domain_params.cpp domain_params.h)

target_link_libraries(dstu4145
    Boost::boost)

target_compile_options(dstu4145 PUBLIC "/std:c++latest")
