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

set (KDMACHINESTATE_SRCS
    kdmachinestate/kdmachinestate.cpp
    kdmachinestate/poweruproutine.cpp
    kdmachinestate/powerdownroutine.cpp
    kdmachinestate/bootroutine.cpp
    kdmachinestate/runodt.cpp)

target_sources (${TARGET} PRIVATE ${KD11COMMON_SRCS} ${KDMACHINESTATE_SRCS})
target_sources (sim-11-test PRIVATE ${KD11COMMON_SRCS} ${KDMACHINESTATE_SRCS})