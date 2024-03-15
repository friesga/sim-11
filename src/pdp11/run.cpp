#include "pdp11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void PDP_11::run ()
{
	// Start the CPU state machine, with the CPU starting either at the
	// address specified in the loaded file or at the standard boot address.
	if (cmdLineOptions_.load_file) 
		processor_->start (loadFile ());
	else
		processor_->start ();
}