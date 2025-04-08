set (RLV12_SRCS
    calccrc.cpp
    rlv12.cpp
    rlcsplusdrivestatus.cpp
    read.cpp
    setdone.cpp
    writeword.cpp
    getstatuscmd.cpp)

target_sources (${TARGET} PRIVATE ${RLV12_SRCS})
target_sources (sim-11-test PRIVATE ${RLV12_SRCS})

add_subdirectory (rlv12command)
add_subdirectory (rl01_02)
add_subdirectory (cmdprocessor)