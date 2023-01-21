#ifndef _IRQRECORD_H_
#define _IRQRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct IrqRecord {};

enum class IrqRecordType
{
    IRQ_OK,
    IRQ_FAIL,
    IRQ_SIG
};

// Specialization of the TraceRecord for the TraceIrq record
template <>
class TraceRecord<IrqRecord>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<IrqRecord> record);

	IrqRecordType type_;
	u16	vector_;

public:
	TraceRecord ();
	TraceRecord (IrqRecordType type, u16 vector);
    Magic magic () {return Magic::IRQ0;}
};

// Constructors for the TraceRecord<IrqRecord> type
inline TraceRecord<IrqRecord>::TraceRecord ()
	:
	type_ {IrqRecordType::IRQ_OK},
	vector_ {0}
{}

inline TraceRecord<IrqRecord>::TraceRecord (IrqRecordType type, u16 vector)
	:
	type_ {type},
	vector_ {vector}
{}


#endif // _IRQRECORD_H_