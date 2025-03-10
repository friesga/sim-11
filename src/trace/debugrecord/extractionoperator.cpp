#include "debugrecord.h"
#include "../tracefileinstream/tracefileinstream.h"

#include <stdexcept>

using std::invalid_argument;

// Definition of the overloaded operator>>() function to read TraceDuration
// records from the tracefile. These records contain a fixed member as
// defined in the class plus a variable length character string.
TracefileInStream &operator>> (TracefileInStream &is, TraceRecord<DebugRecord> &record)
{
    // Read the fixed part of the record
    is.read (reinterpret_cast<char *> (&record), sizeof (record.length_));

    // Read the variable length memory from the trace file in
    // the buffer and transform it into a string.
    size_t constexpr maxStringSize = 128;

    if (record.length_ > maxStringSize)
        throw invalid_argument ("String in DebugRecord too long");

    char buffer[maxStringSize] {0};
    is.read (buffer, record.length_);
    record.msg_ = buffer;

    return is;
}