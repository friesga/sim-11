#ifndef _DURATIONRECORD_H_
#define _DURATIONRECORD_H_

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
struct TimeRecord {};

// Specialization of the TraceRecord for the TraceDuration record
template <>
class TraceRecord<TimeRecord>
{
    friend TracefileOutStream& operator<< (TracefileOutStream& tos, 
        TraceRecord<TimeRecord> record);
    friend TracefileInStream &operator>> (TracefileInStream &is,
        TraceRecord<TimeRecord> &record);
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<TimeRecord> record);

    high_resolution_clock::duration duration_;
    size_t length_;
    string msg_;

public:
	TraceRecord ();
	TraceRecord (string msg, high_resolution_clock::duration duration);
    Magic magic () {return Magic::TIME;}
    string str ();
};

inline TraceRecord<TimeRecord>::TraceRecord ()
	:
    duration_ {},
    length_ {0},
    msg_ {}
{}

inline TraceRecord<TimeRecord>::TraceRecord (string msg, 
    high_resolution_clock::duration duration)
	:
    duration_ {duration},
    length_ {msg.length ()},
    msg_ {msg}
{}

inline string TraceRecord<TimeRecord>::str ()
{
    typedef std::chrono::duration<int, 
        std::ratio_multiply<std::chrono::hours::period, std::ratio<8>>::type> Days;

    Days days = std::chrono::duration_cast<Days> (duration_);
    duration_ -= days;
    auto hours = std::chrono::duration_cast<std::chrono::hours> (duration_);
    duration_ -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes> (duration_);
    duration_ -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds> (duration_);
    duration_ -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds> (duration_);
    duration_ -= milliseconds;
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds> (duration_);
    duration_ -= microseconds;
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds> (duration_);

    // Use operator+= to append strings as that has a far better performance
    // than using string concatenation. See 
    // https://stackoverflow.com/questions/18892281/most-optimized-way-of-concatenation-in-strings
    //
    string result = msg_;
    result += ": ";
    result += to_string (hours.count ());
    result += ':';
    result += to_string (minutes.count ());
    result += ':';
    result += to_string (seconds.count ());
    result += '.';
    result += to_string (milliseconds.count ());
    result += '.';
    result += to_string (microseconds.count ());
    result += '.';
    result += to_string (nanoseconds.count ());
    return result;
}

#endif // !_DURATIONRECORD_H_
