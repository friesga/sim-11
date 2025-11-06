#ifndef _IRQRECORD_H_
#define _IRQRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "bus/interruptrequest/interruptrequest.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct IrqRecord {};

enum class IrqRecordType
{
    IRQ_REQUEST,
    IRQ_GRANT
};

// Specialization of the TraceRecord for the TraceIrq record
template <>
class TraceRecord<IrqRecord>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<IrqRecord> record);

	IrqRecordType type_;
	u16	vector_;
	unsigned int intrpReqId_;

public:
	TraceRecord ();
	TraceRecord (IrqRecordType type, const InterruptRequest& interruptRequest);
    Magic magic () {return Magic::IRQ0;}
};

// Constructors for the TraceRecord<IrqRecord> type
inline TraceRecord<IrqRecord>::TraceRecord ()
	:
	type_ {IrqRecordType::IRQ_REQUEST},
	vector_ {0},
	intrpReqId_ {0}
{}

inline TraceRecord<IrqRecord>::TraceRecord (IrqRecordType type,
	const InterruptRequest& interruptRequest)
	:
	type_ {type},
	vector_ {interruptRequest.vector ()},
	intrpReqId_ {interruptRequest.id ()}
{}


#endif // _IRQRECORD_H_