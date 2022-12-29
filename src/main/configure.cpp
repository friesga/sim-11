#include "main.h"
#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "lsi11/lsi11.h"
#include "console/console.h"
#include "configdata/configprocessor/configprocessor.h"

#include <memory>		// For make_unique
#include <cstring>		// For memset()

using std::shared_ptr;
using std::make_shared;
using std::string;

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void Main::configureDevices (char const *configurationFile)
{
	msv11_ = make_shared<MSV11D> ();
	dlv11_ = make_shared<DLV11J> ();
	bdv11_ = make_shared<BDV11> ();

	// Load device configuration from the configuration file
	if (configurationFile)
	{
		ConfigData configProcessor;
		iniparser::File ft;

		if (!ft.load (configurationFile))
			throw "Error: cannot open file " + string(configurationFile);

		try
		{
			configProcessor.process (ft);
		}
		catch (std::invalid_argument const &except)
		{
			throw "Error in configuration file: " + string(except.what());
		}

		// Retrieve the configuration as specified in the configuration file
		DeviceConfig *deviceConfig = configProcessor.getConfig ();

		// Configure the devices specified in the configuration file
		configureRXV21 (deviceConfig->rxConfig);
		configureRLV12 (deviceConfig->rlConfig);
	}
	// Create the BA11-N bezel after configuration of the devices to prevent
	// the BA11 window popping up and closing if the system is misconfigured
    ba11_n_ = std::make_unique<BA11_N> (lsi11_.bus);

	// The Console class reads characters and sends them to the dlv11
	console_ = Console::create (dlv11_);

	lsi11_.bus.installModule (1, msv11_);
	lsi11_.bus.installModule (2, rlv12_);
	lsi11_.bus.installModule (3, rxv21_);
	lsi11_.bus.installModule (4, dlv11_);
	lsi11_.bus.installModule (5, bdv11_);
	lsi11_.reset ();
}

// Configure the RXV21 if it is specified in the configuration file
void Main::configureRXV21 (RxConfig *rxConfig)
{
	if (rxConfig != nullptr)
		rxv21_ = make_shared<RXV21> (rxConfig);
}

// Configure the RLV12 and attached units if specified in the
// configuration file
void Main::configureRLV12 (RlConfig *rlConfig)
{
	if (rlConfig)
	{
		rlv12_ = make_shared<RLV12> (rlConfig);

		// Attach files to the RL units
		for (size_t unitNumber = 0; 
			unitNumber < rlConfig->numUnits; ++unitNumber)
		{
			RlUnitConfig rlUnitConfig = rlConfig->rlUnitConfig[unitNumber];

			if (rlv12_->unit(unitNumber)->configure (rlUnitConfig) != StatusCode::OK)
				throw "Error attaching " + rlUnitConfig.fileName;
		}
	}
}