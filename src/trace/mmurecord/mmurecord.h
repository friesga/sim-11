#ifndef _MMURECORD_H_
#define _MMURECORD_H_

#include "types.h"
#include "../recordheader.h"
#include "busaddress/busaddress.h"

#include <iomanip>

// Definition of the type to discriminate the trace records in the template
// classes.
struct MMURecord {};

// Specialization of the TraceRecord for the MMURecord record
template <>
class TraceRecord<MMURecord>
{
    friend std::ostream& operator<< (std::ostream& os,
        TraceRecord<MMURecord> record);

    u32 address_;
    BusAddress::Width width_;

public:
    TraceRecord ();
    TraceRecord (BusAddress address);
    Magic magic () { return Magic::MMUA; }
};

// Default constructor for the MMU record
inline TraceRecord<MMURecord>::TraceRecord ()
    :
    address_ {0},
    width_ {BusAddress::Width::_16Bit}
{}

inline TraceRecord<MMURecord>::TraceRecord (BusAddress address)
    :
    address_ {address},
    width_ {static_cast <u16> (address.width ())}
{}

#endif // _MMURECORD_H_