set (BA11L_SRCS
    ba11_l.cpp)

target_sources (${TARGET} PRIVATE ${BA11L_SRCS})
target_sources (sim-11-test PRIVATE ${BA11L_SRCS})
