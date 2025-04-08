set (BDV11_SRCS
    bdv11.cpp
    bdv11_rom.cpp
    kdf11b_rom.cpp)

target_sources (${TARGET} PRIVATE ${BDV11_SRCS})
target_sources (sim-11-test PRIVATE ${BDV11_SRCS})