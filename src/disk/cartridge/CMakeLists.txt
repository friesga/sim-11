set (CARTRIDGE_SRCS     
    cartridge.cpp
    createfile.cpp
    openreadwrite.cpp
    createbadblocktable.cpp
    fileposition.cpp
    readdatafromsector.cpp
    writedatatosector.cpp)

target_sources (${TARGET} PRIVATE ${CARTRIDGE_SRCS})
target_sources (sim-11-wasm PRIVATE ${CARTRIDGE_SRCS})
target_sources (sim-11-test PRIVATE ${CARTRIDGE_SRCS})