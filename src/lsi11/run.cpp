#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void LSI11::run ()
{
	/*
	if (cmdLineOptions_.load_file) 
		loadFile ();
	else
		kd11_->powerUpRoutine ();
	*/
		
	// Start the CPU state machine
	kd11_->run ();
}