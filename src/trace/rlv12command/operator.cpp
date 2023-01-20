#include "rlv12command.h"

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<TraceRLV12Command> record)
{
    os << "[RLV12] command: " << 
        record.getCommandName () << '\n';
    return os;
}