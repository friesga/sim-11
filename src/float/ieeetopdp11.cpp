#include "float.h"
#include "dataformat.h"

#include <cstdint>

// Convert a IEEE 754 32-bit unsigned integet to a double word in
// PDP-11 FIS format.
bool Float::IEEEtoPDP11 (uint32_t const ieeeIn, 
    uint16_t *high, uint16_t *low)
{
    union
    { 
        unsigned short i[2];
        unsigned int l;
    } pdp11part;

    unsigned int mantisse;
    int exponent_;

    if ((ieeeIn & ~SIGN_BIT) == 0)
    {
        // Set IEEE +-zero [exponent_=mantisse=0] to VAX zero [s=exponent_=mantisse=0]
        pdp11part.l = 0;      
    }
    else if ((exponent_ = (ieeeIn & IEEE_S_EXPONENT_MASK)) == IEEE_S_EXPONENT_MASK)
    {
        // PDP11's have no equivalents for IEEE +-Infinity and +-NaN [exponent_=all-1's]
        // raise (SIGFPE);
        
        // Fixup to PDP-11 +-extrema [exponent_=all-1's] with zero mantissa_ 
        pdp11part.l = (ieeeIn & SIGN_BIT) | PDP11_F_EXPONENT_MASK;

        return false;
    }
    else
    {
        // Obtain the biased IEEE exponent_ and IEEE mantissa_
        exponent_ >>= MANTISSA_SIZE;              
        mantisse = ieeeIn & MANTISSA_MASK;           

        if (exponent_ == 0)
        {
            // Denormalized [exponent_=0, mantisse<>0] 
            // Adjust representation from 2**(1-bias) to 2**(exponent_-bias)
            mantisse <<= 1; 
            while ((mantisse & HIDDEN_BIT) == 0)
            {
                mantisse <<= 1;
                // Adjust exponent_
                exponent_ -= 1;                                    
            }
            
            // Adjust mantissa_ to hidden-bit form 
            mantisse &= MANTISSA_MASK;         
        }

        if ((exponent_ += EXPONENT_ADJUSTMENT) <= 0)
        {
            // Silent underflow_
            pdp11part.l = 0;
            return false;
        }
        else if (exponent_ > (2 * PDP11_F_EXPONENT_BIAS - 1))
        {
            // Overflow; fixup to PDP-11 +-extrema [exponent_=mantisse=all-1's]
            // raise (SIGFPE);
            pdp11part.l = (ieeeIn & SIGN_BIT) | ~SIGN_BIT;
            return false;
        }
        else
        {
            // PDP-11 normalized form [exponent_>0] (both mantissas are 23 bits)
            pdp11part.l = (ieeeIn & SIGN_BIT) | 
                (exponent_ << MANTISSA_SIZE) | mantisse;
        }
    }

    *high = pdp11part.i[1];
    *low  = pdp11part.i[0];
    return true;
}
