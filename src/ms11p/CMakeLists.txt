    set (MS11P_SRCS
        ms11p.cpp
        checkbits.cpp
        errorcorrection.cpp
        read.cpp
        write.cpp
        single_error.cpp
        double_error.cpp)

    target_sources (${TARGET} PRIVATE ${MS11P_SRCS})
    target_sources (sim-11-test PRIVATE ${MS11P_SRCS})
