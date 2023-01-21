#ifndef _DLV11RECORD_H_
#define _DLV11RECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <fstream>
#include <string>
#include <sstream>

using std::string;
using std::stringstream;

// Definition of the type to discriminate the trace records in the template
// classes.
struct DLV11Record {};

enum class TraceDLV11Type
{
	DLV11_RX,
	DLV11_TX,
	DLV11_RDY,
	DLV11_SEI,
	DLV11_CLI
};

// Specialization of the TraceRecord for the TraceDLV11 record
template <>
class TraceRecord<DLV11Record>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<DLV11Record> record);

	TraceDLV11Type type_;
	u8 channel_;
	char value_;

	stringstream createMessage (string action);

public:
	TraceRecord ();
	TraceRecord (TraceDLV11Type type, u8 channel, char value);
    Magic magic () {return Magic::DLV1;}
};

inline TraceRecord<DLV11Record>::TraceRecord ()
	:
	type_ {TraceDLV11Type::DLV11_RX},
	channel_ {0},
	value_ {0}
{}

inline TraceRecord<DLV11Record>::TraceRecord (TraceDLV11Type type, u8 channel, char value)
	:
	type_ {type},
	channel_ {channel},
	value_ {value}
{}

#endif // !_DLV11RECORD_H_
