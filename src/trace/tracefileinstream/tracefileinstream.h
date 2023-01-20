#ifndef _TRACEFILEINSTREAM_H_
#define _TRACEFILEINSTREAM_H_

#include "../tracerecord.h"

#include <fstream>
#include <memory>

using std::shared_ptr;

using std::ifstream;

// Definition of an overloaded ifstream class to read a trace file
class TracefileInStream : public ifstream
{
	// The operator>> function is declared friend so it is known by users
	// of the class.
	template <typename T>
	friend TracefileInStream &operator>> (TracefileInStream &is, 
		TraceRecord<T> &record);

	char const *header_;
	size_t headerSize_;

	bool isHeader (shared_ptr<char[]> buffer);

public:
	TracefileInStream (char const *filename);
};

// Definition of the overloaded operator>>() function to read TraceRecords from
// the tracefile.
template <typename T>
TracefileInStream &operator>> (TracefileInStream &is, TraceRecord<T> &record)
{
    is.read (reinterpret_cast<char *> (&record),
		sizeof (TraceRecord<T>));
    return is;
}



#endif // !_TRACEFILEINSTREAM_H_
