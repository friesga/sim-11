#ifndef _INIPROCESSOR_H_
#define _INIPROCESSOR_H_

#include "iniparser.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "../sectionprocessor/deviceconfigprocessor.h"
#include "configdata/msv11config/msv11config.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

using std::map;
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::vector;

// This class processes a configuration file in ini-format and transforms
// it into a system configuration, consisting of a vector of device
// configurations.
//
// Take the following actions to add a device:
// 1. Create a _DEVICE_Config class. The _DEVICE_Config class will hold the
//    device's attributes,
// 2. Create a _DEVICE_Processor class. This class will process the 
//    the section in the ini-file for the device. The class needs a
//    constructor, a processValue() function to incorporate the passed
//    Value in the configuration and a getConfig() function returning the
//	  DeviceConfiguration,
// 3. Add a create_DEVICE_Processor function to this (IniProcessor) class.
//    This function is a factory for the creation of a _DEVICE_Processor
//    object,
// 4. Add an entry to the deviceConfigProcessorFactories map in this class. This
//    maps a section name in the ini-file to the create_DEVICE_Processor
//    function and thereby to the _DEVICE_Processor function processing
//    that section.
//
class IniProcessor
{
public:
    void process (string filename);
	void process (iniparser::File iniFile);
	vector<DeviceConfig> &getSystemConfig ();

private:
	// Define the system configuration as a vector of (shared) pointers
	// to device configurations.
	vector<DeviceConfig> systemConfig {};

	typedef unique_ptr<DeviceConfigProcessor> (IniProcessor::*DeviceConfigProcessorFactory) ();

    map<string, DeviceConfigProcessorFactory> deviceConfigProcessorFactories =
	{
		{"BA11-N",  &IniProcessor::createBA11_NProcessor},
		{"BDV11",   &IniProcessor::createBDV11Processor},
		{"DLV11-J", &IniProcessor::createDLV11Processor},
		{"MSV11",   &IniProcessor::createMSV11Processor},
		{"RL",      &IniProcessor::createRLProcessor},
		{"RXV21",   &IniProcessor::createRXProcessor},
		{"KD11-NA", &IniProcessor::createKD11_NAProcessor},
		{"KDF11-A", &IniProcessor::createKDF11_AProcessor},
		{"KDF11-B", &IniProcessor::createKDF11_BProcessor},
		{"MS11-P",  &IniProcessor::createMS11_PProcessor},
		{"BA11-L",  &IniProcessor::createBA11_LProcessor}
	};

    void processSection (iniparser::Section* section);
	unique_ptr<DeviceConfigProcessor> createBA11_NProcessor ();
	unique_ptr<DeviceConfigProcessor> createBDV11Processor ();
	unique_ptr<DeviceConfigProcessor> createDLV11Processor ();
	unique_ptr<DeviceConfigProcessor> createMSV11Processor ();
	unique_ptr<DeviceConfigProcessor> createRLProcessor ();
	unique_ptr<DeviceConfigProcessor> createRXProcessor ();
	unique_ptr<DeviceConfigProcessor> createKD11_NAProcessor ();
	unique_ptr<DeviceConfigProcessor> createKDF11_AProcessor ();
	unique_ptr<DeviceConfigProcessor> createKDF11_BProcessor ();
	unique_ptr<DeviceConfigProcessor> createMS11_PProcessor ();
	unique_ptr<DeviceConfigProcessor> createBA11_LProcessor ();
};

#endif // !_INIPROCESSOR_H_
