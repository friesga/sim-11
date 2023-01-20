#ifndef _TRACEBUS_H_
#define _TRACEBUS_H_

#include "types.h"
#include "../recordheader.h"

#include <iomanip>

// Definition of the type to discriminate the trace records in the template
// classes.
struct TraceBus {};

enum class TraceBusType
{
    Read,
    Write,
    ReadFail,
    WriteFail,
    BusReset
};

// Specialization of the TraceRecord for the TraceBus record
template <>
class TraceRecord<TraceBus>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<TraceBus> record);

	TraceBusType type_;
	u16	address_;
	u16	value_;
	u16	pad_;

public:
	TraceRecord ();
	TraceRecord (TraceBusType type, u16 address, u16 value);
    Magic magic () {return Magic::BUS0;}
};

// Default constructor for the TraceBus record
inline TraceRecord<TraceBus>::TraceRecord ()
	:
    type_ {TraceBusType::Read},
	address_ {0},
    value_ {0},
	pad_ {0}
{}


inline TraceRecord<TraceBus>::TraceRecord (TraceBusType type,
    u16 address, u16 value)
	:
    type_ {type},
	address_ {address},
    value_ {value},
	pad_ {0}
{}

#endif // !_TRACEBUS_H_
