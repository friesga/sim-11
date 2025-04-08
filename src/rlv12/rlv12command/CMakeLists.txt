set (RLV12COMMAND_SRCS
    rlv12command.cpp)

target_sources (${TARGET} PRIVATE ${RLV12COMMAND_SRCS})
target_sources (sim-11-test PRIVATE ${RLV12COMMAND_SRCS})