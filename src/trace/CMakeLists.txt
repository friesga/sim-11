set (TRACE_SRCS
    trace.cpp
    tracefileoutstream/tracefileoutstream.cpp 
    tracefileoutstream/header.cpp
    clockrecord/tracefileinsertoperator.cpp
    rlv12registersrecord/tracefileinsertoperator.cpp
    memorydumprecord/tracefileinsertoperator.cpp 
    debugrecord/tracefileinsertoperator.cpp)

target_sources (${TARGET} PRIVATE ${TRACE_SRCS})
target_sources (sim-11-test PRIVATE ${TRACE_SRCS})
target_sources (tracereader PRIVATE ${TRACE_SRCS})