set (CARTRIDGE_SRCS     
    cartridge.cpp
    createfile.cpp
    openreadonly.cpp
    openreadwrite.cpp
    createbadblocktable.cpp
    fileposition.cpp
    readdatafromsector.cpp
    writedatatosector.cpp
    writeprotect.cpp)

target_sources (${TARGET} PRIVATE ${CARTRIDGE_SRCS})
target_sources (sim-11-wasm PRIVATE ${CARTRIDGE_SRCS})
target_sources (sim-11-test PRIVATE ${CARTRIDGE_SRCS})