#ifndef _TRACERLV12COMMAND_H_
#define _TRACERLV12COMMAND_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../rxv21disk/rxv21disk.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct TraceRLV12Command {};

// Specialization of the TraceRecord for the TraceRLV12Command record
// The rx2cs member currently isn't used.
template <>
class TraceRecord<TraceRLV12Command>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<TraceRLV12Command> record);

	u16 command_;

public:
	TraceRecord ();
	TraceRecord (u16 command);
    Magic magic () {return Magic::RL2C;}

	const char* getCommandName ();
};


inline TraceRecord<TraceRLV12Command>::TraceRecord ()
	:
	command_ {0}
{}

inline TraceRecord<TraceRLV12Command>::TraceRecord (u16 command)
	:
	command_ {command}
{}


#endif // !_TRACERLV12COMMAND_H_
