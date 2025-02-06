#include "ms11p.h"

using std::popcount;

// The check bits are generated according to a modified Hamming code. The
// check bits are generated as either an xor or a nxor for the eight set data 
// bits in the mask corresponding to the specific check bit. Every bit in
// the given word is first masked with check bit mask and then xor'ed or
// nxor'ed with that mask. The result must than be made an even or odd number
// of set bits.
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
    return (isEven ((word & mask) ^ mask) ? 0 : 1);
}

// Odd parity is determined for a NXOR of the given word and mask
u8 MS11P::oddParity (u16 word, u16 mask)
{
    return (isOdd (~((word & mask) ^ mask)) ? 0 : 1);
}

bool MS11P::isEven (u16 word)
{
    return popcount (word) % 2 == 0;
}

bool MS11P::isOdd (u16 word)
{
    return popcount (word) % 2 != 0;
}

// When the inhibit mode is set, the inhibit mode pointer points to an
// memory area which is inhibited from going into diagnostic check mode.
// When the inhibit mode pointer is set to 0 the first 16K are being
// inhibited, when set to 1 the second 16K of memory are inhibited.
bool MS11P::inhibited (BusAddress address)
{
    return csr_.inhibitModeEnable && 
        ((!csr_.inhibitModePointer && address < 040000) ||
          (csr_.inhibitModePointer && address >= 040000 && address < 0100000));
}