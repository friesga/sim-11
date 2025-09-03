set (IMAGEDATA_SRCS
    openraster/openraster.cpp
    openraster/parsestackxml.cpp
    openraster/readfiledata.cpp
    png/png.cpp)

target_sources (${TARGET} PRIVATE ${IMAGEDATA_SRCS})
target_sources (sim-11-test PRIVATE ${IMAGEDATA_SRCS})