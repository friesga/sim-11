set (BA11N_SRCS
    ba11_n.cpp)

target_sources (${TARGET} PRIVATE ${BA11N_SRCS})
target_sources (sim-11-test PRIVATE ${BA11N_SRCS})
