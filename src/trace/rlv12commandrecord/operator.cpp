#include "rlv12commandrecord.h"

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<RLV12CommandRecord> record)
{
    os << "[RLV12] command: " << 
        record.getCommandName () << '\n';
    return os;
}