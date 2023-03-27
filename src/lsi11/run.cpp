#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void LSI11::run ()
{
	if (cmdLineOptions_.load_file) 
		loadFile ();

    running_ = true;

	if (cmdLineOptions_.halt) 
	{
		kd11_->cpu().runState = 0;
		bus_.setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::False);
	} 
	else if (!cmdLineOptions_.halt && !cmdLineOptions_.load_file) 
	{
		kd11_->cpu().runState = 1;
		bus_.setSignal (Qbus::Signal::SRUN, Qbus::SignalValue::True);
	}
		
	while (running_) 
	{
		unsigned int i;
		
		for(i = 0; i < 1000; i++)
		{
			step ();
			kd11_->step ();
		}

		if ((kd11_->cpu().runState == 0 && cmdLineOptions_.exit_on_halt) ||
			bus_.signalIsSet (Qbus::Signal::EXIT))
		{
			/* make sure ODT finishes its prompt */
			for(i = 0; i < 32; i++)
			{
				step();
				kd11_->step ();
			}

			running_ = false;
		}
	}
}