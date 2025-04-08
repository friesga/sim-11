set (CMDLINEOPTIONS_SRCS 
    cmdlineoptions.cpp
    data.cpp)

target_sources (${TARGET} PRIVATE ${CMDLINEOPTIONS_SRCS})
target_sources (sim-11-test PRIVATE ${CMDLINEOPTIONS_SRCS})
