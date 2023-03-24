#include <stdlib.h>
#include <string.h>

#include "trace/trace.h"
#include "lsi11.h"

LSI11::LSI11 (CmdLineOptions const &cmdLineOptions)
	:
	kd11_ {nullptr},
	msv11_ {nullptr},
	rxv21_ {nullptr},
	rlv12_ {nullptr},
	dlv11_ {nullptr},
	bdv11_ {nullptr},
	ba11_n_ {nullptr},
	cmdLineOptions_ {cmdLineOptions},
	running_ {false}
{}

LSI11::~LSI11 ()
{
	delete kd11_;
	delete msv11_;
	delete dlv11_;
	delete bdv11_;
	delete rxv21_;
	delete rlv12_;
}

// Reset all devices on the bus
void LSI11::reset ()
{
	bus_.reset();
}

void LSI11::step ()
{
	bus_.step ();
}
