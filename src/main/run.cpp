#include "main.h"
#include "cmdlineoptions/cmdlineoptions.h"

void Main::run ()
{
	if (cmdLineOptions_.bootstrap)
		console_->sendString (odt_input);

    running_ = true;

	if (cmdLineOptions_.halt) 
	{
		kdv11_->cpu().runState = 0;
		lsi11_.bus.setProcessorRunning (false);
	} 
	else if (!cmdLineOptions_.bootstrap && 
		!cmdLineOptions_.halt && !cmdLineOptions_.load_file) 
	{
		kdv11_->cpu().runState = 1;
		lsi11_.bus.setProcessorRunning (true);
	}
		
	while (running_) 
	{
		unsigned int i;
		
		for(i = 0; i < 1000; i++)
		{
			lsi11_.step ();
			kdv11_->step (&lsi11_.bus);
		}

		if ((kdv11_->cpu().runState == 0 && 
				cmdLineOptions_.exit_on_halt) ||
			!ba11_n_->isRunning ())
		{
			/* make sure ODT finishes its prompt */
			for(i = 0; i < 32; i++)
			{
				lsi11_.step();
				kdv11_->step (&lsi11_.bus);
			}

			running_ = false;
		}

		dlv11_->step();
	}
}