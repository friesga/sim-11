set (RK11D_SRCS
    rk11d/actionprocessor.cpp
    rk11d/executeread.cpp
    rk11d/executeseek.cpp
    rk11d/executewrite.cpp
    rk11d/hardwarepoll.cpp
    rk11d/misc.cpp
    rk11d/processaction.cpp
    rk11d/read.cpp
    rk11d/rk11d.cpp
    rk11d/writeword.cpp
    rk05/commands.cpp
    rk05/rk05.cpp
    rk05/createbezel.cpp
    rk05/drivethread.cpp
    rk05/statemachine.cpp)

target_sources (${TARGET} PRIVATE ${RK11D_SRCS})
target_sources (sim-11-test PRIVATE ${RK11D_SRCS})