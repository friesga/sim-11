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

struct MemoryDumpRecord;
struct RLV12RegistersRecord;
struct ClockRecord;
struct DebugRecord;

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
		TraceRecord<MemoryDumpRecord> record);
	friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<RLV12RegistersRecord> record);
	friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
		TraceRecord<ClockRecord> record);
	friend TracefileOutStream& operator<< (TracefileOutStream& tos,
		TraceRecord<DebugRecord> record);

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
	// file is accurate, except for CpuStep recors as flushing these records
	// on every write slows down the simulator speed to an unacceptable level.
	// This is a bit of a hack but the alternative is to create another 
	// operator<<() overload which results in duplicate code.
	if (record.magic () != Magic::CPU0)
		tos.flush ();

	tos.limitFileSize ();
    return tos;
}

#endif // !_TRACEFILEOUTSTREAM_H_

