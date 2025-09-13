set (IMAGEDATA_SRCS
    openrasterfile/openrasterfile.cpp
    openrasterfile/parsestackxml.cpp
    openrasterfile/readfiledata.cpp
    png/png.cpp)

target_sources (${TARGET} PRIVATE ${IMAGEDATA_SRCS})
target_sources (sim-11-test PRIVATE ${IMAGEDATA_SRCS})