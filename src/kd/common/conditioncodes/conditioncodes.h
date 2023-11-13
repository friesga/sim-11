#ifndef _CONDITIONCODES_H_
#define _CONDITIONCODES_H_

#include "conddata/conddata.h"
#include "types.h"

struct ConditionCodes
{
    CondData<bool> N {};
    CondData<bool> Z {};
    CondData<bool> V {};
    CondData<bool> C {};

    // These bits are in reversed order as they appear in the PSW. The first
    // bit is the least significant bit.
    struct ConditionCodeBits
    {
        int C_:1 = 0;
        int V_:1 = 0;
        int Z_:1 = 0;
        int N_:1 = 0;
        int filler: 12 = 0;
    };

    union Mask
    {
        // The mask value
        ConditionCodeBits bits;
        uint16_t mask;

        operator uint16_t () { return mask; };
    };

    // The mask "set" contains the new value of the condition code bits for
    // those bits having a 1 in the affected mask.
    Mask set {.bits {.C_ = C.valueOr (0), .V_ = V.valueOr (0), .Z_ = Z.valueOr (0), .N_ = N.valueOr (0)}};
    Mask affected {.bits {.C_ = C.hasValue (), .V_ = V.hasValue (), .Z_ = Z.hasValue (), .N_ = N.hasValue ()}};;

    u16 updatedConditionCodes (u16 oldPSW);

};

// Calculate the new value of the psw
//
// Thruth table:
// PSW           0  0  0  0   1  1  1  1
// set.mask      0  0  1  1   0  0  1  1
// affected.mask 0  1  0  1   0  1  0  1
// result        0  0  0  1   1  0  1  1
// 
// The resulting formula to calculate the result is:
// ((~psw & 017) & set.mask & affected.mask) | 
//        (psw & ~affected.mask) |
//        (psw & affected.mask & set.mask)
//
// This can be simplified to:
// (affected.mask & set.mask) | (psw & ~affected.mask)
//
inline u16 ConditionCodes::updatedConditionCodes (u16 psw)
{
    return (affected.mask & set.mask) | (psw & ~affected.mask);
}

#endif // _CONDITIONCODES_H_