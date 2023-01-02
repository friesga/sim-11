#include "main.h"
#include "cmdlineoptions/cmdlineoptions.h"

void Main::run ()
{
	if (cmdLineOptions_.bootstrap)
		console_->sendString (odt_input);

    running_ = true;

	if (cmdLineOptions_.halt) 
	{
		lsi11_.kd11.cpu().runState = 0;
		lsi11_.bus.setProcessorRunning (false);
	} 
	else if (!cmdLineOptions_.bootstrap && 
		!cmdLineOptions_.halt && !cmdLineOptions_.load_file) 
	{
		lsi11_.kd11.cpu().runState = 1;
		lsi11_.bus.setProcessorRunning (true);
	}
		
	while (running_) 
	{
		unsigned int i;
		
		for(i = 0; i < 1000; i++)
			lsi11_.step ();

		if ((lsi11_.kd11.cpu().runState == 0 && 
				cmdLineOptions_.exit_on_halt) ||
			!ba11_n_->isRunning ())
		{
			/* make sure ODT finishes its prompt */
			for(i = 0; i < 32; i++)
				lsi11_.step();

			running_ = false;
		}

		dlv11_->step();
		rxv21_->step();
	}
}