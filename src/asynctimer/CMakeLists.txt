set (ASYNCTIMER_SRCS
    asynctimer.cpp)

target_sources (${TARGET} PRIVATE ${ASYNCTIMER_SRCS})
target_sources (sim-11-test PRIVATE ${ASYNCTIMER_SRCS})