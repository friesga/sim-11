set (KW11L_SRCS
    kw11l.cpp)

target_sources (${TARGET} PRIVATE ${KW11L_SRCS})
target_sources (sim-11-wasm PRIVATE ${KW11L_SRCS})
target_sources (sim-11-test PRIVATE ${KW11L_SRCS})