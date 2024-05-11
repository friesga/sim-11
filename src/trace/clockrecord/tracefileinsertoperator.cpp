#include "clockrecord.h"

#include "../tracefileoutstream/tracefileoutstream.h"

// Overloaded operator<< function to write time records to a trace file.
// This is a custom function as the ClockRecord contains a fixed and a
// variable part.
TracefileOutStream& operator<< (TracefileOutStream& tos, 
    TraceRecord<ClockRecord> record)
{
    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ tos.traceFileMutex_ };

    // Write the record header, using the magic provided by the record
	TraceRecord<RecordHeader> recordHeader (record.magic ());
	tos.write (reinterpret_cast<char const *>(&recordHeader), 
			sizeof(recordHeader));

    // Write the record to the tracefile without the msg_ data member.
    tos.write (reinterpret_cast<char const *>(&record), 
        sizeof (record.duration_) + sizeof (record.length_));

    // Then write the message
    tos.write (reinterpret_cast<char const *>(record.msg_.c_str()), 
        record.msg_.length());

    // Flush the buffer immediately so when the application crashes the trace
	// file is accurate.
	tos.flush ();
	tos.limitFileSize ();
    return tos;
}