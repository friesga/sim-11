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
		kdv11_->cpu().runState = 0;
		bus.setProcessorRunning (false);
	} 
	else if (!cmdLineOptions_.bootstrap && 
		!cmdLineOptions_.halt && !cmdLineOptions_.load_file) 
	{
		kdv11_->cpu().runState = 1;
		bus.setProcessorRunning (true);
	}
		
	while (running_) 
	{
		unsigned int i;
		
		for(i = 0; i < 1000; i++)
		{
			step ();
			kdv11_->step (&bus);
		}

		if ((kdv11_->cpu().runState == 0 && 
				cmdLineOptions_.exit_on_halt) ||
			!ba11_n_->isRunning ())
		{
			/* make sure ODT finishes its prompt */
			for(i = 0; i < 32; i++)
			{
				step();
				kdv11_->step (&bus);
			}

			running_ = false;
		}

		dlv11_->step();
	}
}