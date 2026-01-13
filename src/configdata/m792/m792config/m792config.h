#ifndef _BM792_H_
#define _BM792_H_

#include "types.h"
#include "devicetype.h"
#include "conddata/conddata.h"

// The M792 is a 32-word read-only-memory (ROM).
// 
// Several options for this module were available:
// - M792-YA - Serial line (KL11/DL11), high-speed paper tape reader (PC11)
// - M792-YB - TC11 DECtape and disks (RC11, RF11, RK11, RP11)
// - M792-YC - CR11 punched card reader
// - MR11-DB - A two board set, consisting of the BM792-YD and -YE, it
//             supported the same devices as the -YB, with the addition
//             of the TM11 magnetic tape
// - M792-YF - DECtape and RF11 and RK11 disks
// - M792-YH - TA11 casette magnetic tape
// - M792-YJ - Special TM11 loader
// - M792-YK - Special version for the VT20, using its second serial line
// - M792-YL - RX11 floppy disk controller
// 
// Source: https://gunkies.org/wiki/BM792_ROM
//
// The M792 is also designated as BM792.
// 
// For now only the M792-YB options is supported.
//
struct M792Config : public DeviceType<BusType::Unibus>
{
    enum class Option
    {
        M792_YA,
        M792_YB,
        M792_YC,
        M792_YD,
        M792_YE,
        M792_YF,
        M792_YH,
        M792_YJ,
        M792_YK,
        M792_YL
    };

    CondData<Option> option {};
};

#endif // _BM792_H_