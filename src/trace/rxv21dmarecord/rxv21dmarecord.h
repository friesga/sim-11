#ifndef _RXV21DMARECORD_H_
#define _RXV21DMARECORD_H_

#include "types.h"
#include "../tracerecord.h"
#include "../recordheader.h"
#include "../rxv21diskrecord/rxv21diskrecord.h"

#include <fstream>

// Definition of the type to discriminate the trace records in the template
// classes.
struct RXV21DmaRecord {};

// Specialization of the TraceRecord for the RXV21Dma record
template <>
class TraceRecord<RXV21DmaRecord>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<RXV21DmaRecord> record);

	RXV21DiskCmd type_;
	u16	rx2wc_;
	u16	rx2ba_;

public:
	TraceRecord ();
	TraceRecord (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba);
    Magic magic () {return Magic::RX2D;}
};

inline TraceRecord<RXV21DmaRecord>::TraceRecord ()
	:
	type_ {RXV21DiskCmd::RXV21_READ},
	rx2wc_ {0},
	rx2ba_ {0}
{}


inline TraceRecord<RXV21DmaRecord>::TraceRecord (RXV21DiskCmd type, u16 rx2wc, u16 rx2ba)
	:
	type_ {type},
	rx2wc_ {rx2wc},
	rx2ba_ {rx2ba}
{}


#endif // !_RXV21DMARECORD_H_
