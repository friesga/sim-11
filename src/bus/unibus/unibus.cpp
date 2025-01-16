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

