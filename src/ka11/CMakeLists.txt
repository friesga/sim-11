set (KA11_SRCS
    ka11.cpp
    ky11_a/ky11_a.cpp
    ky11_a/addressregister/addressregister.cpp)

target_sources (${TARGET} PRIVATE ${KA11_SRCS})
target_sources (sim-11-test PRIVATE ${KA11_SRCS})