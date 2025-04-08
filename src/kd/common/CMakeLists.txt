set (KD11COMMON_SRCS
    operandlocation/operandlocation.cpp
    operandlocation/emptyoperandlocation/emptyoperandlocation.cpp
    operandlocation/registeroperandlocation/registeroperandlocation.cpp
    operandlocation/memoryoperandlocation/memoryoperandlocation.cpp
    instructionformats/nooperandinstruction/nooperandinstruction.cpp
    instructionformats/singleoperandinstruction/singleoperandinstruction.cpp
    instructionformats/doubleoperandinstruction/doubleoperandinstruction.cpp
    instructionformats/fisinstruction/fisinstruction.cpp
    instructionformats/ccinstruction/ccinstruction.cpp
    instructionformats/branchinstruction/branchinstruction.cpp
    instructionformats/eisinstruction/eisinstruction.cpp
    instructionformats/markinstruction/markinstruction.cpp
    instructionformats/sobinstruction/sobinstruction.cpp
    operanddecoderfactory/operanddecoderfactory.cpp
    operanddecoders/baseoperanddecoder/baseoperanddecoder.cpp
    operanddecoders/singleoperanddecoder/singleoperanddecoder.cpp
    operanddecoders/branchdecoder/branchdecoder.cpp
    operanddecoders/doubleoperanddecoder/doubleoperanddecoder.cpp
    operanddecoders/eisdecoder/eisdecoder.cpp
    instructiondecoder/instructiondecoder.cpp
    instructiondecoder/opcodetables.cpp
    executor/executor.cpp
    odt/operatorconsoleaccess/operatorconsoleaccess.cpp)

set (KD11CONTROLLOGIC_SRCS
    controllogic/controllogic.cpp
    controllogic/poweruproutine.cpp
    controllogic/powerdownroutine.cpp
    controllogic/bootroutine.cpp
    controllogic/runodt.cpp
    controllogic/signalreceivers.cpp
    controllogic/statemachine.cpp)

target_sources (${TARGET} PRIVATE ${KD11COMMON_SRCS} ${KD11CONTROLLOGIC_SRCS})
target_sources (sim-11-test PRIVATE ${KD11COMMON_SRCS} ${KD11CONTROLLOGIC_SRCS})