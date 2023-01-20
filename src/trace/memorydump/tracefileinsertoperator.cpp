#include "memorydump.h"

// Overloaded operator<< function to write MemoryDump records to a trace file
TracefileOutStream& operator<< (TracefileOutStream& tos, 
    TraceRecord<MemoryDump> record)
{
    // Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ tos.traceFileMutex_ };

    // Write the record header, using the magic provided by the record
	TraceRecord<RecordHeader> recordHeader (record.magic ());
	tos.write (reinterpret_cast<char const *>(&recordHeader), 
			sizeof(recordHeader));

    // Write the record to the tracefile without the ptr_ data member.
    // The alignment of the shared_ptr member causes that the size of
    // members to write cannot be calculated by subtracting the shared_ptr
    // size from the record size.
    tos.write (reinterpret_cast<char const *>(&record), 
        sizeof (record.address_) + sizeof (record.length_));

    // Then write the memory dump
	tos.write (reinterpret_cast<char const *>(record.ptr_.get()), 
	    	record.length_);

    // Flush the buffer immediately so when the application crashes the trace
	// file is accurate.
	tos.flush ();
	tos.limitFileSize ();
    return tos;
}