set (MM11E_SRCS
    mm11e.cpp)

target_sources (${TARGET} PRIVATE ${MM11E_SRCS})
target_sources (sim-11-wasm PRIVATE ${MM11E_SRCS})
target_sources (sim-11-test PRIVATE ${MM11E_SRCS})