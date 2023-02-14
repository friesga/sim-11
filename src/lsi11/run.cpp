#include "lsi11.h"
#include "cmdlineoptions/cmdlineoptions.h"

void LSI11::run ()
{
	if (cmdLineOptions_.load_file) 
		loadFile ();

	if (cmdLineOptions_.bootstrap)
		console_->sendString (odt_input);

    running_ = true;

	if (cmdLineOptions_.halt) 
	{
		kd11_->cpu().runState = 0;
		bus_.setProcessorRunning (false);
	} 
	else if (!cmdLineOptions_.bootstrap && 
		!cmdLineOptions_.halt && !cmdLineOptions_.load_file) 
	{
		kd11_->cpu().runState = 1;
		bus_.setProcessorRunning (true);
	}
		
	while (running_) 
	{
		unsigned int i;
		
		for(i = 0; i < 1000; i++)
		{
			step ();
			kd11_->step ();
		}

		if ((kd11_->cpu().runState == 0 && 
				cmdLineOptions_.exit_on_halt) ||
			!ba11_n_->isRunning ())
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