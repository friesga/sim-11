set (BA11_SRCS
    ba11c/ba11cconfig/ba11cconfig.cpp
    ba11c/ba11cprocessor/ba11cprocessor.cpp
    ba11l/ba11lconfig/ba11lconfig.cpp
    ba11l/ba11lprocessor/ba11lprocessor.cpp
    ba11n/ba11nconfig/ba11nconfig.cpp
    ba11n/ba11nprocessor/ba11nprocessor.cpp)

target_sources (${TARGET} PRIVATE ${BA11_SRCS})
target_sources (sim-11-test PRIVATE ${BA11_SRCS})