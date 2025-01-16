#include "unibus.h"
#include "trace/trace.h"

// On the Unibus and QBus reads are always word-sized (and from an even
// address) and writes may be either word or byte sized (with byte-sized
// writes using the upper or lower byte depending on whether the address
// is odd or even). When the CPU needs a byte it reads a word and ignores
// the portion of the word it isn't interested in.
// 
// Source: https://retrocomputing.stackexchange.com/questions/13262/how-movb-tstb-and-all-byte-instructions-works-in-odd-address-read-in-pdp11
//
CondData<u16> Unibus::read (BusAddress address)
{
	return {};
}

CondData<u16> Unibus::dmaRead (BusAddress address)
{
    return {};
}