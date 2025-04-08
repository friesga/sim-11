set (M9312_SRCS
    m9312.cpp
    romimages.cpp)

target_sources (${TARGET} PRIVATE ${M9312_SRCS})
target_sources (sim-11-test PRIVATE ${M9312_SRCS})