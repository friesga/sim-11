set (CONCEPTS_SRCS
    audioframe/audioframe.cpp
    geometry/geometry.cpp
    geometry/lbn.cpp
    geometry/lbntodiskaddress.cpp
    playlist/playlist.cpp)

target_sources (${TARGET} PRIVATE ${CONCEPTS_SRCS})
target_sources (sim-11-wasm PRIVATE ${CONCEPTS_SRCS})
target_sources (sim-11-test PRIVATE ${CONCEPTS_SRCS})
target_sources (tracereader PRIVATE ${CONCEPTS_SRCS})