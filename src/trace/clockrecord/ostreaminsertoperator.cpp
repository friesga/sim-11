#include "clockrecord.h"

// Overloaded operator<< function to print TraceDuration records
std::ostream& operator<< (std::ostream& os, TraceRecord<ClockRecord> record)
{
    os << "[CLOCK] " << record.str () << '\n';

    return os;
}