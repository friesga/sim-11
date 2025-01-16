#include "trace/trace.h"
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
	:
	processorRunning_ {false}
{
	ourKey_ = BINIT().subscribe (bind (&Qbus::BINITReceiver, this, _1));
}

CondData<u16> Qbus::read (BusAddress address)
{
	BusDevice *module;

	// Prevent read's from odd addresses
	address &= 0xFFFFFFFE;

	if ((module = configurationHandler_.responsibleModule (address)) != nullptr)
	{
		u16 value;
		if (module->read (address, &value) == StatusCode::OK)
		{
			trace.bus (BusRecordType::Read, address, value);
			return value;
		}
		else
			return {};
	}

	return {};
}


// The bus itself is defined as a BINIT signal receiver too as it not
// only has to pass on the signal to all subscribed devices, but has to
// perform actions itself too.
void Qbus::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

