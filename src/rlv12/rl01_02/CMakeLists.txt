set (RL0102_SRCS
    configure.cpp
    rl01_02.cpp
    available.cpp
    readdata.cpp
    writedata.cpp
    readheader.cpp
    seek.cpp
    updateheadposition.cpp
    drivethread.cpp
    statemachine.cpp
    createbezel.cpp)

target_sources (${TARGET} PRIVATE ${RL0102_SRCS})
target_sources (sim-11-test PRIVATE ${RL0102_SRCS})