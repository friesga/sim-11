set (IMAGEDATA_SRCS
    openrasterfile/openrasterfile.cpp
    openrasterfile/parsestackxml.cpp
    openrasterfile/readfiledata.cpp
    pngimage/pngimage.cpp)

target_sources (${TARGET} PRIVATE ${IMAGEDATA_SRCS})
target_sources (sim-11-test PRIVATE ${IMAGEDATA_SRCS})