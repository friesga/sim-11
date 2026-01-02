set (KD11COMMON_SRCS
    operandlocation/operandlocation.cpp
    operandlocation/emptyoperandlocation/emptyoperandlocation.cpp
    operandlocation/registeroperandlocation/registeroperandlocation.cpp
    operandlocation/memoryoperandlocation/memoryoperandlocation.cpp
    odt/operatorconsoleaccess/operatorconsoleaccess.cpp)

set (KDMACHINESTATE_SRCS
    kdmachinestate/kdmachinestate.cpp
    kdmachinestate/poweruproutine.cpp
    kdmachinestate/powerdownroutine.cpp
    kdmachinestate/bootroutine.cpp
    kdmachinestate/runodt.cpp)

target_sources (${TARGET} PRIVATE ${KD11COMMON_SRCS} ${KDMACHINESTATE_SRCS})
target_sources (sim-11-test PRIVATE ${KD11COMMON_SRCS} ${KDMACHINESTATE_SRCS})