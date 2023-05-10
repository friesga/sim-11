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
		
	while (true) 
	{
		// bus_.step ();
		// kd11_->step ();
		kd11_->run ();

		if (bus_.EXIT())
			break;
	}
}