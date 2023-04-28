#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void LSI11::run ()
{
	if (cmdLineOptions_.load_file) 
		loadFile ();
	else
		kd11_->powerUpRoutine ();
		
	while (true) 
	{
		bus_.step ();
		kd11_->step ();

		if (bus_.signalIsSet (Qbus::Signal::EXIT))
			break;
	}
}