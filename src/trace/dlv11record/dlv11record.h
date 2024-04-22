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

enum class DLV11RecordType
{
	DLV11_RESET,
	DLV11_RX,
	DLV11_TX,
	DLV11_XMIT_RDY,
	DLV11_SET_TXI,
	DLV11_SET_RXI,
	DLV11_CL_TXI,
	DLV11_CL_RXI,
	DLV11_OVERRUN,
	DLV11_READER_ENABLE
};

// Specialization of the TraceRecord for the TraceDLV11 record
template <>
class TraceRecord<DLV11Record>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<DLV11Record> record);

	DLV11RecordType type_;
	u8 channel_;
	char value_;

	stringstream createMessage (string action);

public:
	TraceRecord ();
	TraceRecord (DLV11RecordType type, u8 channel, char value);
    Magic magic () {return Magic::DLV1;}
};

inline TraceRecord<DLV11Record>::TraceRecord ()
	:
	type_ {DLV11RecordType::DLV11_RX},
	channel_ {0},
	value_ {0}
{}

inline TraceRecord<DLV11Record>::TraceRecord (DLV11RecordType type, u8 channel, char value)
	:
	type_ {type},
	channel_ {channel},
	value_ {value}
{}

#endif // !_DLV11RECORD_H_
