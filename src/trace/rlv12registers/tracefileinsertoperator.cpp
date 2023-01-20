#include "rlv12registers.h"
#include "../tracefileoutstream/tracefileoutstream.h"

// Overloaded operator<< function to write RLV21Registers records to
// a trace file. This is a custom function as the RLV21Register record
// contains a fixed and a variable part.
TracefileOutStream& operator<< (TracefileOutStream& tos, 
    TraceRecord<RLV12Registers> record)
{
    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ tos.traceFileMutex_ };

    // Write the record header, using the magic provided by the record
	TraceRecord<RecordHeader> recordHeader (record.magic ());
	tos.write (reinterpret_cast<char const *>(&recordHeader), 
			sizeof(recordHeader));

    // Write the record to the tracefile without the msg_ data member.
    tos.write (reinterpret_cast<char const *>(&record), 
        sizeof (record.rlcs_) + sizeof(record.rlba_) + sizeof(record.rlda_) +
        sizeof (record.rlmpr_) + sizeof(record.rlbae_) + sizeof (record.length_));

    // Then write the message 
    tos.write (reinterpret_cast<char const *>(record.msg_.c_str()), 
        record.msg_.length());

    // Flush the buffer immediately so when the application crashes the trace
	// file is accurate.
	tos.flush ();
	tos.limitFileSize ();
    return tos;
}