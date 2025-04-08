set (BUS_SRCS     
    qbus/qbus.cpp
    qbus/read.cpp
    qbus/writebyte.cpp
    qbus/writeword.cpp
    qbus/bussignals.cpp
    qbus/businterrupts.cpp
    qbus/busconfiguration.cpp
    unibus/unibus.cpp
    unibus/read.cpp
    unibus/writeword.cpp
    unibus/writebyte.cpp
    unibus/bussignals.cpp
    unibus/businterrupts.cpp
    unibus/busconfiguration.cpp
    signalhandler/signalhandler.cpp
    configurationhandler/configurationhandler.cpp
    configurationhandler/responsiblemodule.cpp
    iterator/iterator.cpp
    interrupthandler/interrupthandler.cpp)

target_sources (${TARGET} PRIVATE ${BUS_SRCS})
target_sources (sim-11-test PRIVATE ${BUS_SRCS})