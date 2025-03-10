#include "ms11p.h"

#include <stdexcept>

using std::logic_error;

u16 MS11P::correctSingleError (u16 data, u8 syndromeBits)
{
    u8 errorBit = syndromeDecodeTable_[syndromeBits].bit;
    return data ^ (1 << errorBit);
}

// This function decodes the syndrome bits to detect if a single or double
// error occurred.
MS11P::BitError MS11P::detectedErrors (u8 syndromeBits)
{ 
    SyndromeDecodeKey key = syndromeDecodeTable_[syndromeBits].key;


    switch (key)
    {
        case SyndromeDecodeKey::NoError:
            return BitError::None;

        case SyndromeDecodeKey::DataBit:
        case SyndromeDecodeKey::CheckBit:
            return BitError::Single;

        case SyndromeDecodeKey::TwoErrors:
        case SyndromeDecodeKey::MultipleErrors:
            return BitError::Double;
    }

    throw logic_error ("Should not happen");
}