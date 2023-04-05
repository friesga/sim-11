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
		step ();
		kd11_->step ();

		if ((kd11_->cpu().currentCpuState () == CpuState::HALT && 
				cmdLineOptions_.exit_on_halt) ||
			bus_.signalIsSet (Qbus::Signal::EXIT))
		{
			break;
		}
	}
}