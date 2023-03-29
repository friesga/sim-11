#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void LSI11::run ()
{
	if (cmdLineOptions_.load_file) 
		loadFile ();

	if (cmdLineOptions_.halt) 
	{
		kd11_->cpu ().halt ();
	} 
	else if (!cmdLineOptions_.halt && !cmdLineOptions_.load_file) 
	{
		// Hm, this smells. Start running with a loaded file via a proceed
		// seems odd. This should be (and is?) handled in load_file().
		kd11_->cpu().proceed ();
	}
		
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