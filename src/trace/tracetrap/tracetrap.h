#ifndef _TRACETRAP_H_
#define _TRACETRAP_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <fstream>


// Definition of the type to discriminate the trace records in the template
// classes.
struct TraceTrap {};

enum class TraceTrapCause
{
	TRAP,
	TRAP_ILL,
	TRAP_RADDR,
	TRAP_T
};

// Specialization of the TraceRecord for the TraceTrap record
template <>
class TraceRecord<TraceTrap>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<TraceTrap> record);

	TraceTrapCause cause_;
	u16	vector_;

    const char* getTrapName (int vector);

public:
	TraceRecord ();
	TraceRecord (TraceTrapCause type, u16 vector);
    Magic magic () {return Magic::TRAP;}
};

// Constructors for the TraceRecord<TraceTrap> type
inline TraceRecord<TraceTrap>::TraceRecord ()
	:
	cause_ {TraceTrapCause::TRAP},
	vector_ {0}
{}

inline TraceRecord<TraceTrap>::TraceRecord (TraceTrapCause cause, u16 vector)
	:
	cause_ {cause},
	vector_ {vector}
{}


#endif // _TRACETRAP_H_