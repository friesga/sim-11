#include "rxv21commandrecord.h"

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<RXV21CommandRecord> record)
{
    os << "[RXV21] Execute command: " << 
        TraceRecord<RXV21CommandRecord>::getCommandName (record.type_) << '\n';
    return os;
}