#include "trace/trace.h"

#include <string>
#include <gtest/gtest.h>

using std::string;

// Definition of the test fixture
class TraceTest : public ::testing::Test
{};

// There is just one test for the fixed length TraceRecords as these records
// share the tracefile insertion and extraction operator functions.
TEST_F (TraceTest, CpuEventExtractEqualsInsertion)
{
    TracefileOutStream tracefileOutStream ("tracefile");
    CpuEventRecordType const cpuEventType {CpuEventRecordType::CPU_HALT};
    u16 value {1};
    tracefileOutStream << TraceRecord<CpuEventRecord> (cpuEventType, value);
    tracefileOutStream.close ();

    TracefileInStream tracefileInStream ("tracefile");
    TraceRecord<RecordHeader> recordHeader;
    tracefileInStream >> recordHeader;
    EXPECT_EQ (recordHeader.magic(), Magic::CPU1);

    TraceRecord<CpuEventRecord> cpuEvent;
    tracefileInStream >> cpuEvent;

    EXPECT_EQ (cpuEvent.type(), cpuEventType);
    EXPECT_EQ (cpuEvent.value(), value);
}

// The RLV12Registers TraceRecord contains a fixed number of members plus 
// a variable length string.
TEST_F (TraceTest, RLV12RegistersExtractEqualsInsertion)
{
    TracefileOutStream tracefileOutStream ("tracefile");

    string msg ("message");
    tracefileOutStream << TraceRecord<RLV12RegistersRecord> (msg, 0, 1, 2, 3, 4);
    tracefileOutStream.close ();

    TracefileInStream tracefileInStream ("tracefile");
    TraceRecord<RecordHeader> recordHeader;
    tracefileInStream >> recordHeader;
    EXPECT_EQ (recordHeader.magic(), Magic::RL2A);

    TraceRecord<RLV12RegistersRecord> rlv12Registers;
    tracefileInStream >> rlv12Registers;

    EXPECT_EQ (rlv12Registers.rlcs(), 0);
    EXPECT_EQ (rlv12Registers.rlba (), 1);
    EXPECT_EQ (rlv12Registers.rlda (), 2);
    EXPECT_EQ (rlv12Registers.rlmpr (), 3);
    EXPECT_EQ (rlv12Registers.rlbae (), 4);
    EXPECT_EQ (rlv12Registers.length (), msg.length ());
    EXPECT_TRUE (rlv12Registers.msg () == msg);
}

// The RLV12Registers TraceRecord contains a fixed number of members plus 
// a variable number of bytes. 
TEST_F (TraceTest, MemoryDump)
{
    TracefileOutStream tracefileOutStream ("tracefile");

    u8 memory[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    size_t const memSize {sizeof (memory)};
    tracefileOutStream << TraceRecord<MemoryDumpRecord> (memory, 3, memSize);
    tracefileOutStream.close ();

    TracefileInStream tracefileInStream ("tracefile");
    TraceRecord<RecordHeader> recordHeader;
    tracefileInStream >> recordHeader;
    EXPECT_EQ (recordHeader.magic(), Magic::BUS1);

    TraceRecord<MemoryDumpRecord> memoryDump;
    tracefileInStream >> memoryDump;

    EXPECT_EQ (memoryDump.address (), 3);
    EXPECT_EQ (memoryDump.length (), memSize);

    for (size_t index = 0; index < memSize; ++index)
        EXPECT_EQ (memoryDump.ptr ()[index], index);
}