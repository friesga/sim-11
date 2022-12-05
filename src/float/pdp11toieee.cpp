#include "float.h"
#include "dataformat.h"

#include <cstdint>      // For integer types
#include <cstdio>
#include <cstring>
#include <cmath>
#include <limits.h>

// Convert a PDP-11 double word in FIS format to a 32 bit unsigned integer
// in IEEE 754 format.
bool Float::PDP11toIEEE (uint16_t const high, uint16_t const low, 
    uint32_t *ieeeOut)
{
    union
    { 
        unsigned short i[2];
        unsigned int l;
    }  pdp11part;

    unsigned int pdp11part1;
    int exponent;

    pdp11part.i[1] = high;
    pdp11part.i[0] = low;
    pdp11part1 = pdp11part.l;

    if ((exponent = (pdp11part1 & PDP11_F_EXPONENT_MASK)) == 0)
    {
        // If the biased VAX exponent_ is zero [exponent_=0]
        if ((pdp11part1 & SIGN_BIT) == SIGN_BIT)
        {
            // If negative [s=1] 
            // PDP-11 reserved operand fault; fixup to IEEE zero 
            // raise (SIGFPE);
            *ieeeOut = 0;
            return false;
        }
        
        // Set PDP-11 dirty [mantisse<>0] or true [mantisse=0] zero 
        // to IEEE +zero [sign=exponent_=mantisse=0] 
        *ieeeOut = 0;
    }
    else
    {
        // The biased PDP-11 exponent_ is non-zero [exponent_<>0]
        // Obtain the biased PDP-11 exponent_
        exponent >>= MANTISSA_SIZE;               

        // The  biased  PDP-11 exponent_ has to be adjusted to account for the 
        // right shift of the IEEE mantissa_ binary point and the difference 
        // between the biases in their "excess n" exponent_ representations. 
        // If the resulting biased IEEE exponent_ is less than or equal to 
        // zero, the converted IEEE S_float must use subnormal form.         

        if ((exponent -= EXPONENT_ADJUSTMENT) > 0)
        {
            // Use IEEE normalized form [exponent_>0]
            // Both mantissas are 23 bits; adjust the exponent_ field in place
            *ieeeOut = pdp11part1 - IN_PLACE_EXPONENT_ADJUSTMENT;
        }
        else
        {
            // Use IEEE subnormal form [exponent_=0, mantisse>0] 

            // In IEEE subnormal form, even though the biased exponent_ is 0  
            // [exponent_=0], the effective biased exponent_ is 1.  The mantissa_
            // must be shifted right by the number of bits, n, required to
            // adjust the biased exponent_ from its current value, exponent_,
            // to 1.  I.e., exponent_ + n = 1, thus n = 1 - exponent_.  n is
            // guaranteed to be at least 1 [exponent_<=0], which guarantees that
            // the hidden 1.m bit from the original mantissa_ will become
            // visible, and the resulting subnormal mantissa_ will correctly be
            // of the form 0.m.
            *ieeeOut = (pdp11part1 & SIGN_BIT) |
                    ((HIDDEN_BIT | (pdp11part1 & MANTISSA_MASK)) >> (1 - exponent));
        }
    }
    return true;
}

#if 0
// Alternative implementation of PDP11toIEEE() from 
// https://stackoverflow.com/questions/64760137/how-to-display-dec-floating-point-format-given-32-bits-in-ieee-standard
float Float::single_precision_pdp11_to_ieee754 (uint16_t const high, uint16_t const low)
{
    /* mixed-endian: more significant word at lower address,
       but within word less significant byte at lower address
    */
    FloatUnion number;

    number.u32_ = ((uint32_t) high << 2 * CHAR_BIT) | (uint32_t) low;

    uint32_t expo = (number.u32_ >> 23) & 0xff;
    float res;

    if (expo == 0x00) 
        res = copysignf (0.0f, number.f32_);
    else if (expo == 0xff)
    {
        number.u32_ = number.u32_ - (2 << 23); // decrement exponent by 2
        res = number.f32_;
    } 
    else 
        res = 0.25f * number.f32_;
    
    return res;
}
#endif