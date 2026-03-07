#ifndef _RK11EVENTRECORD_H_
#define _RK11EVENTRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "rk/include/rktypes.h"
#include "rk/rk11d/rk11d.h"

#include <fstream>
#include <string>

using std::string;

// Definition of the type to discriminate the trace records in the template
// classes.
struct RK11EventRecord {};

// Specialization of the TraceRecord for the RK11RegistersRecord
template <>
class TraceRecord<RK11EventRecord>
{
	friend std::ostream& operator<< (std::ostream& os,
		TraceRecord<RK11EventRecord> record);

	// Definition of the contents of the record
	RK11D::State state_;
	RK11D::Event event_;
	
public:
	TraceRecord () : state_ {}, event_ {RKTypes::Function {}} {};
	TraceRecord (RK11D::State state, RK11D::Event event)
		: state_(state), event_(event) {};
	Magic magic () { return Magic::RK1E; }

	string getFunctionName (RKTypes::Function function) const;
	string getStateName (RK11D::State state) const;
};

#endif // _RK11EVENTRECORD_H_

