#
# The tracereader sources are located in the src/trace directory to keep them
# organized per trace record type.
#
set (TRACEREADER_SRCS
    ${CMAKE_SOURCE_DIR}/src/trace/tracefileoutstream/tracefileoutstream.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/tracefileoutstream/header.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/tracefileinstream/tracefileinstream.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/clockrecord/extractionoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/cpusteprecord/disas.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/traprecord/gettrapname.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/rxv21commandrecord/getcommandname.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/rxv21errorrecord/geterrorname.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/rlv12registersrecord/extractionoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/rlv12commandrecord/getcommandname.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/cpueventrecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/cpusteprecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/memorydumprecord/extractionoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/busrecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/traprecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/irqrecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/rxv21diskrecord/operator.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/rxv21commandrecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/rxv21dmarecord/operator.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/rxv21errorrecord/operator.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/dlv11record/operator.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/rlv12commandrecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/rlv12registersrecord/ostreaminsertopreator.cpp 
    ${CMAKE_SOURCE_DIR}/src/trace/unibusmaprecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/clockrecord/ostreaminsertoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/memorydumprecord/ostreaminsertoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/aprrecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/mmurecord/operator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/debugrecord/extractionoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/debugrecord/ostreaminsertoperator.cpp
    ${CMAKE_SOURCE_DIR}/src/trace/ms11precord/operator.cpp)

target_sources (sim-11-test PRIVATE ${TRACEREADER_SRCS})
target_sources (tracereader PRIVATE main.cpp ${TRACEREADER_SRCS})