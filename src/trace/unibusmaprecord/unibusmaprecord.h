#ifndef _UNIBUSMAPRECORD_H_
#define _UNIBUSMAPRECORD_H_

#include "types.h"
#include "../recordheader.h"
#include "busaddress/busaddress.h"

#include <iomanip>

// Definition of the type to discriminate the trace records in the template
// classes.
struct UnibusMapRecord {};

// Specialization of the TraceRecord for the TraceBus record
template <>
class TraceRecord<UnibusMapRecord>
{
    friend std::ostream& operator<< (std::ostream& os,
        TraceRecord<UnibusMapRecord> record);

    u32 inputAddress_;
    BusAddress::Width width_;
    u16 registerIndex_;
    u16 low_;
    u16 high_;
    u32 physicalAddress_;

public:
    TraceRecord ();
    TraceRecord (u32 inputAddress, BusAddress::Width width, u16 registerIndex,
        u16 low, u16 high, u32 physicalAddress);
    Magic magic () { return Magic::UBM0; }
};

// Default constructor for the TraceBus record
inline TraceRecord<UnibusMapRecord>::TraceRecord ()
    :
    inputAddress_ {0},
    width_ {BusAddress::Width::_16Bit},
    registerIndex_ {0},
    low_ {0},
    high_ {0},
    physicalAddress_ {0}
{}

inline TraceRecord<UnibusMapRecord>::TraceRecord (u32 inputAddress,
    BusAddress::Width width, u16 registerIndex,
    u16 low, u16 high, u32 physicalAddress)
    :
    inputAddress_ {inputAddress},
    width_ {width},
    registerIndex_ {registerIndex},
    low_ {low},
    high_ {high},
    physicalAddress_ {physicalAddress}
{}

#endif // _UNIBUSMAPRECORD_H_