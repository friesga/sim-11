#include "iniprocessor.h"

#include <string>

using std::string;
using std::invalid_argument;

void IniProcessor::processSection (iniparser::Section* section)
{
	// Get a pointer to the DeviceConfigrocessor factory to be used
	// to create the appropriate DeviceConfigProcessor to process this
	// section and create that DeviceConfigProcessor.
	DeviceConfigProcessorFactory factory = 
		deviceConfigProcessorFactories[section->name()];

	// Check a DeviceConfigProcessor factory exists for this section
	if (factory)
	{
		// Create the appropriate DeviceConfigProcessor to process this section...
		unique_ptr<DeviceConfigProcessor> deviceConfigProcessor = 
			(this->*factory) ();

		// ...and let it process this section.
		deviceConfigProcessor->processSection (section);

		// Add the device configuration to the system configuration
		systemConfig.addDeviceConfig (deviceConfigProcessor->getConfig ());
	}
	else
		throw invalid_argument ("Unsupported section: " + section->name ());
}