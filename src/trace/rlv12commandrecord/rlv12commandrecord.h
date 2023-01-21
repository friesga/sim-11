#ifndef _TRACERLV12COMMANDRECORD_H_
#define _TRACERLV12COMMANDRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../rxv21diskrecord/rxv21diskrecord.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct RLV12CommandRecord {};

// Specialization of the TraceRecord for the TraceRLV12Command record
// The rx2cs member currently isn't used.
template <>
class TraceRecord<RLV12CommandRecord>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<RLV12CommandRecord> record);

	u16 command_;

public:
	TraceRecord ();
	TraceRecord (u16 command);
    Magic magic () {return Magic::RL2C;}

	const char* getCommandName ();
};


inline TraceRecord<RLV12CommandRecord>::TraceRecord ()
	:
	command_ {0}
{}

inline TraceRecord<RLV12CommandRecord>::TraceRecord (u16 command)
	:
	command_ {command}
{}


#endif // !_TRACERLV12COMMANDRECORD_H_
