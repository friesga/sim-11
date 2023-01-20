#include "rxv21command.h"

// Print the given record to the given output stream
std::ostream& operator<< (std::ostream& os, TraceRecord<RXV21Command> record)
{
    os << "[RXV21] Execute command: " << 
        TraceRecord<RXV21Command>::getCommandName (record.type_) << '\n';
    return os;
}