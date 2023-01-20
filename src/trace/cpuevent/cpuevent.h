#ifndef _CPUEVENT_H_
#define _CPUEVENT_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <iomanip>

using std::oct;

// Note: TRC_CPU_ODT_P and TRC_CPU_ODT_G originally had the same value
enum class CpuEventType
{
    CPU_VOID,
    CPU_TRAP,
    CPU_HALT,
    CPU_WAIT,
    CPU_RUN,
    CPU_DBLBUS,
    CPU_ODT_P,
    CPU_ODT_G
};


// This file has to be included in trace.h after definition of the
// TraceRecord class.

// Definition of the type to discriminate the trace records in the template
// classes.
struct CpuEvent {};

// Specialization of the TraceRecord for CpuEvent
template <>
class TraceRecord<CpuEvent>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<CpuEvent> record);

	CpuEventType type_;
	u16	value_;

public:
	TraceRecord ();
	TraceRecord (CpuEventType type, u16 value);

	Magic magic () {return Magic::CPU1;}

    // Definition of accessors to be used in unit tests
    CpuEventType type() { return type_;}
    u16	value() {return value_;}
};

// Default constructor for the CpuEvent record
inline TraceRecord<CpuEvent>::TraceRecord ()
	:
    type_ {CpuEventType::CPU_VOID},
    value_ {0}
{}


inline TraceRecord<CpuEvent>::TraceRecord (CpuEventType type, u16 value)
	:
	type_ {type},
    value_ {value}
{}


#endif // _CPUEVENT_H_