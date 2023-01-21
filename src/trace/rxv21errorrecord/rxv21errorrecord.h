#ifndef _RXV21ERRORRECORD_H_
#define _RXV21ERRORRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct RXV21Error {};

enum class RXV21ErrorType
{
	RXV21_WC_OVFL,
	RXV21_DEN_ERR,
	RXV21_TRACK_NO,
	RXV21_SECT_NO
};

// Specialization of the TraceRecord for the RXV21Error record
template <>
class TraceRecord<RXV21Error>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<RXV21Error> record);

	RXV21ErrorType type_;
	u16	info_;

	const char* getErrorName ();

public:
	TraceRecord ();
	TraceRecord (RXV21ErrorType type, u16 info);
    Magic magic () {return Magic::RX2E;}
};

inline TraceRecord<RXV21Error>::TraceRecord ()
	:
	type_ {RXV21ErrorType::RXV21_DEN_ERR},
	info_ {0}
{}

inline TraceRecord<RXV21Error>::TraceRecord (RXV21ErrorType type, u16 info)
	:
	type_ {type},
	info_ {info}
{}

#endif // !_RXV21ERROR_H_
