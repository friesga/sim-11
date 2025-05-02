set (UNIT_SRCS     
    attach_unit.cpp
    createfile.cpp
    openreadonly.cpp
    openreadwrite.cpp
    createbadblocktable.cpp
    readdatafromsector.cpp
    writeprotect.cpp)

target_sources (${TARGET} PRIVATE ${UNIT_SRCS})
target_sources (sim-11-test PRIVATE ${UNIT_SRCS})