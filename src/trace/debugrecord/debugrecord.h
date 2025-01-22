#ifndef _DEBUGRECORD_H_
#define _DEBUGRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../tracefileoutstream/tracefileoutstream.h"
#include "../tracefileinstream/tracefileinstream.h"

#include <fstream>
#include <string>
#include <chrono>

using std::string;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::time_point;
using std::to_string;

// Definition of the type to discriminate the trace records in the template
// classes.
struct DebugRecord {};

// Specialization of the TraceRecord for the DebugRecord record
template <>
class TraceRecord<DebugRecord>
{
    friend TracefileOutStream& operator<< (TracefileOutStream& tos,
        TraceRecord<DebugRecord> record);
    friend TracefileInStream& operator>> (TracefileInStream& is,
        TraceRecord<DebugRecord>& record);
    friend std::ostream& operator<< (std::ostream& os,
        TraceRecord<DebugRecord> record);

    size_t length_;
    string msg_;

public:
    TraceRecord ();
    TraceRecord (string msg);
    Magic magic () { return Magic::DBUG; }
    string str ();
};

inline TraceRecord<DebugRecord>::TraceRecord ()
    :
    length_ {0},
    msg_ {}
{}

inline TraceRecord<DebugRecord>::TraceRecord (string msg)
    :
    length_ {msg.length ()},
    msg_ {msg}
{}

#endif // !_DEBUGRECORD_H_
