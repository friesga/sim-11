set (RXV21_SRCS 
    rxv21.cpp
    initiatecommand.cpp
    emptybuffer.cpp
    fillbuffer.cpp
    readerrorcode.cpp
    readsector.cpp
    readstatus.cpp
    writesector.cpp)

target_sources (${TARGET} PRIVATE ${RXV21_SRCS})
target_sources (sim-11-test PRIVATE ${RXV21_SRCS})