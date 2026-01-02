set (PROC_COMMON_SRCS
    cpudata/singularregisterset/singularregisterset.cpp
    basemachinestate/basemachinestate.cpp
    basemachinestate/signalreceivers.cpp
    basemachinestate/statemachine.cpp
    composite_cpucontroller/composite_cpucontroller.cpp
    basicprocessorexceptionhandler/basicprocessorexceptionhandler.cpp
    executor/executor.cpp
    pseudommu/pseudommu.cpp
    pseudo_haltmode/pseudo_haltmode.cpp
    operanddecoderfactory/operanddecoderfactory.cpp
    operanddecoders/baseoperanddecoder/baseoperanddecoder.cpp
    operanddecoders/singleoperanddecoder/singleoperanddecoder.cpp
    operanddecoders/branchdecoder/branchdecoder.cpp
    operanddecoders/doubleoperanddecoder/doubleoperanddecoder.cpp
    operanddecoders/eisdecoder/eisdecoder.cpp
    operandlocation/operandlocation.cpp
    operandlocation/emptyoperandlocation/emptyoperandlocation.cpp
    operandlocation/registeroperandlocation/registeroperandlocation.cpp
    operandlocation/memoryoperandlocation/memoryoperandlocation.cpp
    instructiondecoder/instructiondecoder.cpp
    instructiondecoder/opcodetables.cpp
    instructionformats/nooperandinstruction/nooperandinstruction.cpp
    instructionformats/singleoperandinstruction/singleoperandinstruction.cpp
    instructionformats/doubleoperandinstruction/doubleoperandinstruction.cpp
    instructionformats/fisinstruction/fisinstruction.cpp
    instructionformats/ccinstruction/ccinstruction.cpp
    instructionformats/branchinstruction/branchinstruction.cpp
    instructionformats/eisinstruction/eisinstruction.cpp
    instructionformats/markinstruction/markinstruction.cpp
    instructionformats/sobinstruction/sobinstruction.cpp)

target_sources (${TARGET} PRIVATE ${PROC_COMMON_SRCS})
target_sources (sim-11-test PRIVATE ${PROC_COMMON_SRCS})