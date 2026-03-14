set (M792_SRCS
    m792.cpp
    m792romimages.cpp)

target_sources (${TARGET} PRIVATE ${M792_SRCS})
target_sources (sim-11-wasm PRIVATE ${M792_SRCS})
target_sources (sim-11-test PRIVATE ${M792_SRCS})