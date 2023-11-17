#include "timerecord.h"

// Overloaded operator<< function to print TraceDuration records
std::ostream& operator<< (std::ostream& os, TraceRecord<TimeRecord> record)
{
    os << "[TIME] " << record.str () << '\n';

    return os;
}