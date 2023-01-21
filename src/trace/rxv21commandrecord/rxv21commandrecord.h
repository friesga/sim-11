#ifndef _RXV21COMMANDRECORD_H_
#define _RXV21COMMANDRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../rxv21diskrecord/rxv21diskrecord.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct RXV21CommandRecord {};

// Specialization of the TraceRecord for the RXV21Command record
// The rx2cs member currently isn't used.
template <>
class TraceRecord<RXV21CommandRecord>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<RXV21CommandRecord> record);

	RXV21DiskCmd type_;
	u16	rx2cs_;

public:
	TraceRecord ();
	TraceRecord (RXV21DiskCmd type, u16 rx2cs);
    Magic magic () {return Magic::RX2C;}

	// The function getCommandName() is made static and public to make it
	// available from other trace records.
	static const char* getCommandName (RXV21DiskCmd command);
};

inline TraceRecord<RXV21CommandRecord>::TraceRecord ()
	:
	type_ {RXV21DiskCmd::RXV21_EMPTY},
	rx2cs_ {0}
{}

inline TraceRecord<RXV21CommandRecord>::TraceRecord (RXV21DiskCmd type, u16 rx2cs)
	:
	type_ {type},
	rx2cs_ {rx2cs}
{}


#endif // _RXV21COMMANDRECORD_H_