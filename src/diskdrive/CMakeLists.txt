set (UNIT_SRCS     
    attachfile.cpp
    createfile.cpp
    openreadonly.cpp
    openreadwrite.cpp
    createbadblocktable.cpp
    fileposition.cpp
    readdatafromsector.cpp
    writedatatosector.cpp
    writeprotect.cpp
    geometry/geometry.cpp)

target_sources (${TARGET} PRIVATE ${UNIT_SRCS})
target_sources (sim-11-test PRIVATE ${UNIT_SRCS})