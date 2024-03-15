#include <stdlib.h>
#include <string.h>

#include "trace/trace.h"
#include "pdp11.h"

PDP_11::PDP_11 (CmdLineOptions const &cmdLineOptions)
	:
	processor_ {nullptr},
	msv11_ {},
	rxv21_ {nullptr},
	rlv12_ {nullptr},
	dlv11_ {nullptr},
	bdv11_ {nullptr},
	ba11_n_ {nullptr},
	cmdLineOptions_ {cmdLineOptions}
{}

PDP_11::~PDP_11 ()
{
	delete processor_;
	for (MSV11D* msv11d : msv11_)
		delete msv11d;
	delete dlv11_;
	delete bdv11_;
	delete rxv21_;
	delete rlv12_;
}

// Reset the bus and all devices on the bus. The bus will forward the BINIT
// signal to all devices on the bus subscribed to that signal, including
// the bus itself.
void PDP_11::reset ()
{
	bus_.BINIT().cycle ();
}

