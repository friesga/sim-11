#include <stdlib.h>
#include <string.h>

#include "trace/trace.h"
#include "pdp11.h"

PDP_11::PDP_11 (CmdLineOptions const &cmdLineOptions)
	:
	processor_ {nullptr},
	ba11_n_ {nullptr},
	cmdLineOptions_ {cmdLineOptions}
{}

PDP_11::~PDP_11 ()
{
	delete processor_;
	for (auto device : memoryDevices_)
		delete device;

	for (auto device : busDevices_)
		delete device;

	delete bus_;
}

// Reset the bus and all devices on the bus. The bus will forward the BINIT
// signal to all devices on the bus subscribed to that signal, including
// the bus itself.
void PDP_11::reset ()
{
	bus_->BINIT().cycle ();
}

