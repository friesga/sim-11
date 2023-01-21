#ifndef _DURATIONRECORD_H_
#define _DURATIONRECORD_H_

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
struct DurationRecord {};

// Specialization of the TraceRecord for the TraceDuration record
template <>
class TraceRecord<DurationRecord>
{
    friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<DurationRecord> record);
    friend TracefileInStream &operator>> (TracefileInStream &is,
        TraceRecord<DurationRecord> &record);
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<DurationRecord> record);

    u32 durationCount_;     // in microseconds
    size_t length_;
    string msg_;

public:
	TraceRecord ();
	TraceRecord (string msg, u32 durationCount);
    Magic magic () {return Magic::DURA;}
};

inline TraceRecord<DurationRecord>::TraceRecord ()
	:
    durationCount_ {0},
    length_ {0},
    msg_ {}
{}

inline TraceRecord<DurationRecord>::TraceRecord (string msg, u32 durationCount)
	:
    durationCount_ {durationCount},
    length_ {msg.length ()},
    msg_ {msg}
{}

#endif // !_DURATIONRECORD_H_
