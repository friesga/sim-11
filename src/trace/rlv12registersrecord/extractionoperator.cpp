#include "rlv12registersrecord.h"
#include "../tracefileinstream/tracefileinstream.h"

#include <stdexcept>

using std::invalid_argument;

// Definition of the overloaded operator>>() function to read RLV12Registers
// records from the tracefile. These records contain the fixed members as
// defined in the class (minus the msg_) plus a variable length character
// string.
TracefileInStream &operator>> (TracefileInStream &is, 
    TraceRecord<RLV12RegistersRecord> &record)
{
    // Read the fixed part of the record
    is.read (reinterpret_cast<char *> (&record),
        sizeof (record.rlcs_) + sizeof(record.rlba_) + sizeof(record.rlda_) +
        sizeof (record.rlmpr_) + sizeof(record.rlbae_) + sizeof (record.length_));

    // Read the variable length string from the trace file in the buffer
    // and transform it into a string.
    size_t constexpr maxStringSize = 128;

    if (record.length_ > maxStringSize)
        throw invalid_argument ("String in RLV12RegistersRecord record too long");

    char buffer[maxStringSize] {0};
    is.read (buffer, record.length_);
    record.msg_ = buffer;

    return is;
}