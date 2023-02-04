#include "main.h"
#include "ba11_n/ba11_n.h"
#include "rxv21/rxv21.h"
#include "rlv12/rlv12.h"
#include "bdv11/bdv11.h"
#include "dlv11j/dlv11j.h"
#include "msv11d/msv11d.h"
#include "lsi11/lsi11.h"
#include "console/console.h"
#include "configdata/iniprocessor/iniprocessor.h"
#include "configdata/rxv21config/rxv21config.h"

#include <memory>		// For make_unique
#include <cstring>		// For memset()

using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;
using std::string;

// Configure the devices and install them on the bus.
// Accessing a non-configured device will result in a bus time-out
// and the BDV11 boot will halt at address 000010.
void Main::configureDevices ()
{
	// Load device configuration from the configuration file
	if (cmdLineOptions_.config_file)
	{
		IniProcessor configProcessor;
		iniparser::File ft;

		if (!ft.load (cmdLineOptions_.config_file))
			throw "Error: cannot open file " + string(cmdLineOptions_.config_file);

		try
		{
			configProcessor.process (ft);
		}
		catch (std::invalid_argument const &except)
		{
			throw "Error in configuration file: " + string(except.what());
		}

		// Get the device configurations and populate the LSI bus with these devices.
		for (shared_ptr<DeviceConfig> device : configProcessor.getSystemConfig ())
		{
			switch (device->deviceType_)
			{
				case DeviceType::MSV11:
					msv11_ = make_shared<MSV11D> ();
					break;

				case DeviceType::DLV11_J:
					dlv11_ = make_shared<DLV11J> ();
					break;

				case DeviceType::BDV11:
					bdv11_ = make_shared<BDV11> ();
					break;

				case DeviceType::RXV21:
					rxv21_ = make_shared<RXV21> 
						(static_pointer_cast<RXV21Config> (device));
					break;

				case DeviceType::RLV12:
					rlv12_ = make_shared<RLV12>
						(static_pointer_cast<RLConfig> (device));
					break;

				case DeviceType::BA11_N:
					ba11_n_ = std::make_unique<BA11_N> (lsi11_.bus);
					break;

				default:
					throw "Unknown device type in configuration";
			}
		}

	}
	else
	{
		// No configuration file specified; create a bare system with a
		// default configuration and without any files attached.
		msv11_ = make_shared<MSV11D> ();
		dlv11_ = make_shared<DLV11J> ();
		bdv11_ = make_shared<BDV11> ();
		rxv21_ = make_shared<RXV21> ();
		rlv12_ = make_shared<RLV12> ();
		ba11_n_ = std::make_unique<BA11_N> (lsi11_.bus);
	}

	// The Console class reads characters and sends them to the dlv11
	console_ = Console::create (dlv11_);

	lsi11_.bus.installModule (1, msv11_);
	lsi11_.bus.installModule (2, rlv12_);
	lsi11_.bus.installModule (3, rxv21_);
	lsi11_.bus.installModule (4, dlv11_);
	lsi11_.bus.installModule (5, bdv11_);
	lsi11_.reset ();
}
