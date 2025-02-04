#include "ms11p.h"

using std::popcount;

// The check bits are generated according to a modified Hamming code.
u8 MS11P::generateCheckBits (u16 word)
{
    CheckBits checkBits {0};
    checkBits.CX = evenParity (word, cxMask);
    checkBits.C0 = evenParity (word, c0Mask);
    checkBits.C1 = oddParity  (word, c1Mask);
    checkBits.C2 = oddParity  (word, c2Mask);
    checkBits.C4 = evenParity (word, c4Mask);
    checkBits.C8 = evenParity (word, c8Mask);

    return checkBits.value;
}

// Even parity is determined for an XOR of the given word and mask
u8 MS11P::evenParity (u16 word, u16 mask)
{
    return (isEven (word ^ mask) ? 0 : 1);
}

// Odd parity is determined for a NXOR of the given word and mask
u8 MS11P::oddParity (u16 word, u16 mask)
{
    return (isOdd ((word ^ mask) == mask) ? 0 : 1);
}

bool MS11P::isEven (u16 word)
{
    return popcount (word) % 2 == 0;
}

bool MS11P::isOdd (u16 word)
{
    return popcount (word) % 2 != 0;
}