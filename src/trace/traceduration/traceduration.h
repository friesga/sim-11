#ifndef _TRACEDURATION_H_
#define _TRACEDURATION_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../tracefileoutstream/tracefileoutstream.h"
#include "../tracefileinstream/tracefileinstream.h"

#include <fstream>
#include <string>

using std::string;

// Definition of the type to discriminate the trace records in the template
// classes.
struct TraceDuration {};

// Specialization of the TraceRecord for the TraceDuration record
template <>
class TraceRecord<TraceDuration>
{
    friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<TraceDuration> record);
    friend TracefileInStream &operator>> (TracefileInStream &is,
        TraceRecord<TraceDuration> &record);
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<TraceDuration> record);

    u32 durationCount_;     // in microseconds
    size_t length_;
    string msg_;

public:
	TraceRecord ();
	TraceRecord (string msg, u32 durationCount);
    Magic magic () {return Magic::DURA;}
};

inline TraceRecord<TraceDuration>::TraceRecord ()
	:
    durationCount_ {0},
    length_ {0},
    msg_ {}
{}

inline TraceRecord<TraceDuration>::TraceRecord (string msg, u32 durationCount)
	:
    durationCount_ {durationCount},
    length_ {msg.length ()},
    msg_ {msg}
{}

#endif // !_TRACEDURATION_H_
