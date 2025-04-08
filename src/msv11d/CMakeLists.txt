set (MSV11D_SRCS
    msv11d.cpp)

target_sources (${TARGET} PRIVATE ${MSV11D_SRCS})
target_sources (sim-11-test PRIVATE ${MSV11D_SRCS})