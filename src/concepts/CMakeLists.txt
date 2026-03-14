set (CONCEPTS_SRCS     
    geometry/geometry.cpp
    geometry/lbn.cpp
    geometry/lbntodiskaddress.cpp)

target_sources (${TARGET} PRIVATE ${CONCEPTS_SRCS})
target_sources (sim-11-wasm PRIVATE ${CONCEPTS_SRCS})
target_sources (sim-11-test PRIVATE ${CONCEPTS_SRCS})
target_sources (tracereader PRIVATE ${CONCEPTS_SRCS})