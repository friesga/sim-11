#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void LSI11::run ()
{
	// Start the CPU state machine, with the CPU starting either at the
	// address specified in the loaded file or at the standard boot address.
	if (cmdLineOptions_.load_file) 
		kd11_na_->start (loadFile ());
	else
		kd11_na_->start ();
}