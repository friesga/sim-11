set (CHRONO_SRCS
    simulatorclock/simulatorclock.cpp
    alarmclock/alarmclock.cpp
    wakeupcall/alarmwakeupcall/alarmwakeupcall.cpp)

target_sources (${TARGET} PRIVATE ${CHRONO_SRCS})
target_sources (sim-11-test PRIVATE ${CHRONO_SRCS})