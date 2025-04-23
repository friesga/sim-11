set (RK11D_SRCS
    rk11d/rk11d.cpp)

target_sources (${TARGET} PRIVATE ${RK11D_SRCS})
target_sources (sim-11-test PRIVATE ${RK11D_SRCS})