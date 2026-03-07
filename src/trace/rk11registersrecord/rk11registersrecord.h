#ifndef _RK11REGISTERSRECORD_H_
#define _RK11REGISTERSRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "rk/include/rktypes.h"

#include <fstream>
#include <string>

using std::string;

// Definition of the type to discriminate the trace records in the template
// classes.
struct RK11RegistersRecord {};

// Specialization of the TraceRecord for the RK11RegistersRecord
template <>
class TraceRecord<RK11RegistersRecord>
{
	friend std::ostream& operator<< (std::ostream& os,
		TraceRecord<RK11RegistersRecord> record);

	// Definition of the contents of the record
	BusAddress busAddress_ {0};
	RKTypes::RKER rker_ {0};
	RKTypes::RKDS rkds_ {0};
	RKTypes::RKCS rkcs_ {0};
	u16 rkwc_ {0};
	u16 rkba_ {0};
	RKTypes::RKDA rkda_ {0};
	u16 rkdb_ {0};

public:
	TraceRecord ();
	TraceRecord (BusAddress busAddress, RKTypes::RKER rker, RKTypes::RKDS rkds,
		RKTypes::RKCS rkcs, u16 rkwc, u16 rkba, RKTypes::RKDA rkda, u16 rkdb);
	Magic magic () { return Magic::RK1R; }
};

inline TraceRecord<RK11RegistersRecord>::TraceRecord ()
{}

inline TraceRecord<RK11RegistersRecord>::TraceRecord (BusAddress busAddress,
	RKTypes::RKER rker, RKTypes::RKDS rkds, RKTypes::RKCS rkcs, u16 rkwc,
	u16 rkba, RKTypes::RKDA rkda, u16 rkdb)
	:
	busAddress_ {busAddress},
	rker_ {rker},
	rkds_ {rkds},
	rkcs_ {rkcs},
	rkwc_ {rkwc},
	rkba_ {rkba},
	rkda_ {rkda},
	rkdb_ {rkdb}
{}

#endif // _RK11REGISTERSRECORD_H_
