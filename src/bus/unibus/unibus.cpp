#include "unibus.h"

#include <functional>

using std::bind;
using std::placeholders::_1;

Unibus::Unibus ()
{
	ourKey_ = BINIT().subscribe (bind (&Unibus::BINITReceiver, this, _1));
}

// The bus itself is defined as a BINIT signal receiver too as it not
// only has to pass on the signal to all subscribed devices, but has to
// perform actions itself too.
void Unibus::BINITReceiver (bool signalValue)
{
	if (signalValue)
		reset ();
}

// A given address must be mapped if:
// - A Unibus Map option is present, and
// - The address is not in the I/O page, and
// - The address is a 18-bit address, or
// - The address is a 22-bit address with the highest four bits set.
bool Unibus::addressMustBeMapped (BusAddress address)
{
	return unibusMap_ != nullptr && !address.isInIOpage () &&
		((address.width () == BusAddress::Width::_18Bit) ||
		(address.width () == BusAddress::Width::_22Bit &&
			(static_cast<u32> (address) & 017000000) == 017000000));
}