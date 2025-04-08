set (DL11_SRCS
    dlv11j/dlv11j.cpp
    uart/uart.cpp)

target_sources (${TARGET} PRIVATE ${DL11_SRCS})
target_sources (sim-11-test PRIVATE ${DL11_SRCS})