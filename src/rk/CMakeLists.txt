set (RK11D_SRCS
    rk11d/functionprocessor.cpp
    rk11d/driveread.cpp
    rk11d/drivereadheader.cpp
    rk11d/driveseek.cpp
    rk11d/drivewrite.cpp
    rk11d/drivewritecheck.cpp
    rk11d/drivewritelock.cpp
    rk11d/misc.cpp
    rk11d/processfunction.cpp
    rk11d/read.cpp
    rk11d/rk11d.cpp
    rk11d/reportseekcomplete.cpp
    rk11d/steps.cpp
    rk11d/writeword.cpp
    rk05/commands.cpp
    rk05/rk05.cpp
    rk05/createbezel.cpp
    rk05/drivethread.cpp
    rk05/statemachine.cpp)

target_sources (${TARGET} PRIVATE ${RK11D_SRCS})
target_sources (sim-11-wasm PRIVATE ${RK11D_SRCS})
target_sources (sim-11-test PRIVATE ${RK11D_SRCS})