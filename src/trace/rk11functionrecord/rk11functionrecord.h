#ifndef _RK11FUNCTIONRECORD_H_
#define _RK11FUNCTIONRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "rk/include/rktypes.h"

#include <fstream>
#include <string>

using std::string;

// Definition of the type to discriminate the trace records in the template
// classes.
struct RK11FunctionRecord {};

// Specialization of the TraceRecord for the RK11FunctionRecord
template <>
class TraceRecord<RK11FunctionRecord>
{
	friend std::ostream& operator<< (std::ostream& os,
		TraceRecord<RK11FunctionRecord> record);

	RKTypes::Function function_;

public:
	TraceRecord ();
	TraceRecord (RKTypes::Function function);
	Magic magic () { return Magic::RK1F; }

	string getFunctionName () const;
};

inline TraceRecord<RK11FunctionRecord>::TraceRecord ()
	:
	function_ {}
{}

inline TraceRecord<RK11FunctionRecord>::TraceRecord (RKTypes::Function function)
	:
	function_ {function}
{}


#endif // _RK11FUNCTIONRECORD_H_
