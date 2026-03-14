set (DL11_SRCS
    dl11/dl11.cpp
    dlv11j/dlv11j.cpp
    uart/uart.cpp)

target_sources (${TARGET} PRIVATE ${DL11_SRCS})
target_sources (sim-11-wasm PRIVATE ${DL11_SRCS})
target_sources (sim-11-test PRIVATE ${DL11_SRCS})