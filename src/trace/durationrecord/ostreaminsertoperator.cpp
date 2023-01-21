#include "durationrecord.h"

// Overloaded operator<< function to print TraceDuration records
std::ostream& operator<< (std::ostream& os, TraceRecord<DurationRecord> record)
{
    os << "[DURATION] " << record.msg_ << ": " << record.durationCount_ << '\n';

    return os;
}