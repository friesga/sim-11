set (BA11C_SRCS
    ba11_c.cpp)

target_sources (${TARGET} PRIVATE ${BA11C_SRCS})
target_sources (sim-11-test PRIVATE ${BA11C_SRCS})
