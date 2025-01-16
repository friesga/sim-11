#include "qbus.h"

#include <stdlib.h>
#include <string.h>
#include <algorithm>

using std::bind;
using std::placeholders::_1;

// On the Unibus and QBus reads are always word-sized (and from an even
// address) and writes may be either word or byte sized (with byte-sized
// writes using the upper or lower byte depending on whether the address
// is odd or even). When the CPU needs a byte it reads a word and ignores
// the portion of the word it isn't interested in.
// 
// Source: https://retrocomputing.stackexchange.com/questions/13262/how-movb-tstb-and-all-byte-instructions-works-in-odd-address-read-in-pdp11
//
Qbus::Qbus ()
{
	ourKey_ = BINIT().subscribe (bind (&Qbus::BINITReceiver, this, _1));
}

// The bus itself is defined as a BINIT signal receiver too as it not
// only has to pass on the signal to all subscribed devices, but has to
// perform actions itself too.
void Qbus::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

