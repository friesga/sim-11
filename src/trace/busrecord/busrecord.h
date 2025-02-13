#ifndef _BUSRECORD_H_
#define _BUSRECORD_H_

#include "types.h"
#include "../recordheader.h"
#include "busaddress/busaddress.h"

#include <iomanip>

// Definition of the type to discriminate the trace records in the template
// classes.
struct BusRecord {};

enum class BusRecordType
{
    Read,
    Write,
    ReadFail,
    WriteFail,
    BusReset,
    ParityError
};

// Specialization of the TraceRecord for the TraceBus record
template <>
class TraceRecord<BusRecord>
{
    friend std::ostream& operator<< (std::ostream& os, 
        TraceRecord<BusRecord> record);

	BusRecordType type_;
	u32	address_;
    u16 width_;
	u16	value_;

public:
	TraceRecord ();
	TraceRecord (BusRecordType type, BusAddress address, u16 value);
    Magic magic () {return Magic::BUS0;}
};

// Default constructor for the TraceBus record
inline TraceRecord<BusRecord>::TraceRecord ()
	:
    type_ {BusRecordType::Read},
	address_ {0},
    width_ {0},
    value_ {0}
{}


inline TraceRecord<BusRecord>::TraceRecord (BusRecordType type,
    BusAddress address, u16 value)
	:
    type_ {type},
	address_ {address},
    width_ {static_cast<u16> (address.width())},
    value_ {value}
{}

#endif // !_BUSRECORD_H_
