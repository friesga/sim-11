#include "qbus.h"

#include <functional>

using std::bind;
using std::placeholders::_1;

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

