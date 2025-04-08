set (KD11_NA_ODT_SRCS
    kd11_na_odt.cpp
    addressopened.cpp
    atprompt.cpp
    createevent.cpp
    enteringaddress.cpp
    enteringaddressvalue.cpp
    enteringbinarydump.cpp
    enteringregister.cpp
    enteringregistervalue.cpp
    entrypoint.cpp
    exitpoint.cpp
    registeropened.cpp
    startingregister.cpp)

target_sources (${TARGET} PRIVATE ${KD11_NA_ODT_SRCS})
target_sources (sim-11-test PRIVATE ${KD11_NA_ODT_SRCS})