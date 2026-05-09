#include "ms11p.h"

using std::popcount;

u8 MS11P::newCheckBits (BusAddress address, u16 value)
{
    return (csr_.diagnosticCheck && !inhibited (address)) ?
        checkSyndromeBits_ :
        generateCheckBits (value);
}

// The check bits are generated according to a modified Hamming code. The
// check bits are generated as either an xor or a nxor for the eight set data 
// bits in the mask corresponding to the specific check bit. Every bit in
// the given word is first masked with check bit mask and then xor'ed or
// nxor'ed with that mask. The result must than be made an even or odd number
// of set bits. This function reflects EK-MS11P-TM-001 Table 3-1.
u8 MS11P::generateCheckBits (u16 word)
{
    CheckBits checkBits {0};
    checkBits.CX = evenParity (XOR  (word, cxMask));
    checkBits.C0 = evenParity (XOR  (word, c0Mask));
    checkBits.C1 = oddParity  (NXOR (word, c1Mask));
    checkBits.C2 = oddParity  (NXOR (word, c2Mask));
    checkBits.C4 = evenParity (XOR  (word, c4Mask));
    checkBits.C8 = evenParity (XOR  (word, c8Mask));

    return checkBits.value;
}

// Even parity is determined for an XOR of the given word and mask
u8 MS11P::evenParity (u16 word)
{
    return isEven (word) ? 0 : 1;
}

// Odd parity is determined for a NXOR of the given word and mask
u8 MS11P::oddParity (u16 word)
{
    return (isOdd (word) ? 0 : 1);
}

// Definition of the XOR function as defined in EK-MS11P-TM-001 par 3.2.2.
u16 MS11P:: XOR (u16 word, u16 mask)
{
    return (word & mask) ^ mask;
}

// Definition of the NXOR function as defined in EK-MS11P-TM-001 par 3.2.2.
u16 MS11P::NXOR (u16 word, u16 mask)
{
    return ~((word & mask) ^ mask);
}

bool MS11P::isEven (u16 word)
{
    return popcount (word) % 2 == 0;
}

bool MS11P::isOdd (u16 word)
{
    return popcount (word) % 2 != 0;
}

// This function determines if the given address is inhibited (c.q. protected)
// from going into Diagnostic Check Mode.  When the inhibit mode is set, the
// inhibit mode pointer points to an memory area which is inhibited from going
// into diagnostic check mode. When the inhibit mode pointer is set to 0 the
// first 16KW (see ZMSPB0 test 4) are being inhibited, when set to 1 the second
// 16KW of memory are inhibited.
bool MS11P::inhibited (BusAddress address)
{
    return csr_.inhibitModeEnable && 
        (inInhibitedFirst16KW (address) || inInhibitedSecond16KW (address));
}

bool MS11P::inInhibitedFirst16KW (BusAddress address)
{
    u16 const _16KW {16 * 2048};
    return !csr_.inhibitModePointer && address < _16KW;
}

bool MS11P::inInhibitedSecond16KW (BusAddress address)
{
    u16 const _16KW {16 * 2048};
    u16 const _32KW {static_cast<u16> (32 * 2048)};
    return csr_.inhibitModePointer && address >= _16KW && address < _32KW;
}