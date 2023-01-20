#ifndef _TRACEFILEOUTSTREAM_H_
#define _TRACEFILEOUTSTREAM_H_

#include "../recordheader.h"

#include <fstream>
#include <utility>
#include <mutex>

using std::ofstream;
using std::pair;
using std::mutex;
using std::lock_guard;

// Forward declarations to be able to use these in the friend declarations.
template <typename T>
class TraceRecord;

struct MemoryDump;
struct RLV12Registers;
struct TraceDuration;

// Definition of an overloaded ofstream class to be used for writing
// TraceRecords to the tracefile. To this purpose a separate class is
// defined to be able to override operator<<() for writes to the tracefile.
// (operator<<() is overloaded too to print the contents of a TraceRecord.)

class TracefileOutStream : public ofstream
{
	template <typename T>
	friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
		TraceRecord<T> record);
	friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
		TraceRecord<MemoryDump> record);
	friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<RLV12Registers> record);
	friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
		TraceRecord<TraceDuration> record);

	static char const header_[6];
	std::string baseFileName {};
	size_t fileSequenceNumber_ {0};
    size_t recordCounter_ {0};
	mutex traceFileMutex_;

	void limitFileSize ();

public:
	TracefileOutStream () {};
    TracefileOutStream (char const *filename);
	~TracefileOutStream ();
    void open (char const *filename);
	void close ();

	static pair<const char*, size_t> header ();
};

// Definition of the overloaded operator<<() function to write TraceRecords to
// the tracefile.
template <typename T>
TracefileOutStream& operator<< (TracefileOutStream& tos, TraceRecord<T> record)
{
	// Guard against simultaneous trace file writes
    lock_guard<mutex> guard{ tos.traceFileMutex_ };

	// Write the record header, using the magic provided by the record
	TraceRecord<RecordHeader> recordHeader (record.magic ());
	tos.write (reinterpret_cast<char const *>(&recordHeader), 
			sizeof(recordHeader));

	// Then write the record itself
	tos.write (reinterpret_cast<char const *>(&record), 
			sizeof(TraceRecord<T>));

	// Flush the buffer immediately so when the application crashes the trace
	// file is accurate.
	tos.flush ();
	tos.limitFileSize ();
    return tos;
}

#endif // !_TRACEFILEOUTSTREAM_H_

