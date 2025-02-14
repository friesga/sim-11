#ifndef _MS11PRECORD_H
#define _MS11PRECORD_H

#include "types.h"
#include "../recordheader.h"
#include "busaddress/busaddress.h"

#include <iomanip>

// Definition of the type to discriminate the trace records in the template
// classes.
struct MS11_PRecord {};

enum class MS11_PRecordType
{
    ReadCSR,
    WriteCSR,
    ReadMemory,
    WriteMemory,
};

// Specialization of the TraceRecord for the MS11_PRecord record
template <>
class TraceRecord<MS11_PRecord>
{
    friend std::ostream& operator<< (std::ostream& os,
        TraceRecord<MS11_PRecord> record);

    MS11_PRecordType type_;
    u16	csr_;
    BusAddress address_;
    u16	value_;
    u8 checkBits_;

public:
    TraceRecord ();
    TraceRecord (MS11_PRecordType type, u16 csr, BusAddress address,
        u16 value, u8 checkBits);
    Magic magic () { return Magic::MS1P; }
};

// Default constructor for the TraceBus record
inline TraceRecord<MS11_PRecord>::TraceRecord ()
    :
    type_ {MS11_PRecordType::ReadCSR},
    csr_ {0},
    address_ {0},
    value_ {0},
    checkBits_ {0}
{}


inline TraceRecord<MS11_PRecord>::TraceRecord (MS11_PRecordType type,
    u16 csr, BusAddress address, u16 value, u8 checkBits)
    :
    type_ {type},
    csr_ {csr},
    address_ {address},
    value_ {value},
    checkBits_ {checkBits}
{}

#endif // _MS11PRECORD_H_