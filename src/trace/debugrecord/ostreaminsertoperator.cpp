#include "debugrecord.h"

// Overloaded operator<< function to print DebugRecord's
std::ostream& operator<< (std::ostream& os, TraceRecord<DebugRecord> record)
{
    os << "[DEBUG] " << record.msg_ << '\n';

    return os;
}