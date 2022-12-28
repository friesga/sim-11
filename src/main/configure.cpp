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

// void configureRLV12 (DeviceConfig *deviceConfig)
// {}

// Configure the devices specfied in the given configuration file
// on the given lsi.
// ToDo: floppyFileName to be specified in configurati
void Main::configureDevices (char const *configurationFile, 
	char const *floppyFilename)
{
	msv11_ = make_shared<MSV11D> ();
	rxv21_ = make_shared<RXV21> ();
	dlv11_ = make_shared<DLV11J> ();
	bdv11_ = make_shared<BDV11> ();
	
	// Create the BA11-N bezel
    ba11_n_ = std::make_unique<BA11_N> (lsi11_.bus);

	FILE* floppy_file;

	// Allocate memory for the floppy (RXV21) data, open the file with
	// the floppy drive data (if given) and pass the data buffer address
	// to the RXV21 controller.
	// Note that the complete contents of the file is read into the data
	// buffer.
	floppy_ = (u8*) malloc(77 * 26 * 256);
	if (!floppy_) 
		throw "Error: cannot allocate memory for rxv21";

	// Check if a floppy has to be configured
	if (floppyFilename) 
	{
		floppy_file = fopen (floppyFilename, "rb");
		if (!floppy_file) 
		{
			free(floppy_);
			throw "Error: cannot open file " + string(floppyFilename);
		}
		(void) !fread (floppy_, 77 * 26 * 256, 1, floppy_file);
		fclose (floppy_file);
	} 
	else 
		memset (floppy_, 0, 77 * 26 * 256);

	rxv21_->setData (floppy_);

	// Load device configuration
	DeviceConfig *deviceConfig {nullptr};
	ConfigData configProcessor;

	// ToDo: Presence of configuration file is optional
	if (configurationFile)
	{
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
	}

	// Retrieve the configuration as specified in the configuration file
	deviceConfig = configProcessor.getConfig ();

	// RLV12 rlv12 (deviceConfig->rlConfig);
	shared_ptr<RLV12> rlv12 = make_shared<RLV12> (deviceConfig->rlConfig);

	// Attach files to the RL units
	for (size_t unitNumber = 0; 
		unitNumber < deviceConfig->rlConfig->numUnits; ++unitNumber)
	{
		RlUnitConfig rlUnitConfig = 
			deviceConfig->rlConfig->rlUnitConfig[unitNumber];

		if (rlv12->unit(unitNumber)->configure (rlUnitConfig) != StatusCode::OK)
			throw "Error attaching " + rlUnitConfig.fileName;
	}

	// The Console class reads characters and sends them to the dlv11
	console_ = Console::create (dlv11_);

	lsi11_.bus.installModule (1, msv11_);
	lsi11_.bus.installModule (2, rlv12);
	lsi11_.bus.installModule (3, rxv21_);
	lsi11_.bus.installModule (4, dlv11_);
	lsi11_.bus.installModule (5, bdv11_);
	lsi11_.reset ();
}