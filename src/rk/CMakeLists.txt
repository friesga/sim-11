set (RK11D_SRCS
    rk11d/rk11d.cpp
    rk11d/read.cpp
    rk11d/writeword.cpp
    rk05/rk05.cpp
    rk05/createbezel.cpp)

target_sources (${TARGET} PRIVATE ${RK11D_SRCS})
target_sources (sim-11-test PRIVATE ${RK11D_SRCS})