#ifndef _IRQRECORD_H_
#define _IRQRECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct TraceIrq {};

enum class TraceIrqType
{
    IRQ_OK,
    IRQ_FAIL,
    IRQ_SIG
};

// Specialization of the TraceRecord for the TraceIrq record
template <>
class TraceRecord<TraceIrq>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<TraceIrq> record);

	TraceIrqType type_;
	u16	vector_;

public:
	TraceRecord ();
	TraceRecord (TraceIrqType type, u16 vector);
    Magic magic () {return Magic::IRQ0;}
};

// Constructors for the TraceRecord<TraceIrq> type
inline TraceRecord<TraceIrq>::TraceRecord ()
	:
	type_ {TraceIrqType::IRQ_OK},
	vector_ {0}
{}

inline TraceRecord<TraceIrq>::TraceRecord (TraceIrqType type, u16 vector)
	:
	type_ {type},
	vector_ {vector}
{}


#endif // _IRQRECORD_H_