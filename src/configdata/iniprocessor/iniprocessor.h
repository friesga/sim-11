#ifndef _INIPROCESSOR_H_
#define _INIPROCESSOR_H_

#include "iniparser.h"
#include "../sectionprocessor/sectionprocessor.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

using std::map;
using std::shared_ptr;
using std::string;
using std::vector;

// This class processes a configuration file in ini-format and transforms
// it into a system configuration, consisting of a vector of device
// configurations.
//
// Take the following actions to add a device:
// 1. Add the device type to the DeviceType enumeration (in 
//    devicetype/devicetype.h),
// 2. Create a _DEVICE_Config class derived from DeviceConfig, with a
//    constructor calling the DeviceConfig constructor with the device
//    type as added to DeviceType. The _DEVICE_Config class will hold the
//    device's attributes,
// 3. Create a _DEVICE_Processor class. This class will process the 
//    the section in the ini-file for the device. The class needs a
//    constructor, a processValue() function to incorporate the passed
//    Value in the configuration and a getConfig() function returning the
//	  DeviceConfiguration,
// 4. Add a create_DEVICE_Processor function to this (IniProcessor) class.
//    This function is a factory for the creation of a _DEVICE_Processor
//    object,
// 5. Add an entry to the sectionProcessorFactories map in this class. This
//    maps a section name in the ini-file to the create_DEVICE_Processor
//    function and thereby to the _DEVICE_Processor function processing
//    that section.
//
class IniProcessor
{
	// Define the system configuration as a vector of (shared) pointers
	// to device configurations.
	vector<shared_ptr<DeviceConfig>> systemConfig {};

	typedef unique_ptr<SectionProcessor> (IniProcessor::*SectionProcessorFactory) ();

    map<string, SectionProcessorFactory> sectionProcessorFactories =
	{
		{"BA11-N",  &IniProcessor::createBA11_NProcessor},
		{"BDV11",   &IniProcessor::createBDV11Processor},
		{"DLV11-J", &IniProcessor::createDLV11Processor},
		{"MSV11",   &IniProcessor::createMSV11Processor},
		{"RL",      &IniProcessor::createRLProcessor},
		{"RXV21",   &IniProcessor::createRXProcessor}
	};

    void processSection (iniparser::Section* section);
	unique_ptr<SectionProcessor> createBA11_NProcessor ();
	unique_ptr<SectionProcessor> createBDV11Processor ();
	unique_ptr<SectionProcessor> createDLV11Processor ();
	unique_ptr<SectionProcessor> createMSV11Processor ();
	unique_ptr<SectionProcessor> createRLProcessor ();
	unique_ptr<SectionProcessor> createRXProcessor ();

public:
    void process (string filename);
	void process (iniparser::File iniFile);
	vector<shared_ptr<DeviceConfig>> &getSystemConfig ();
};

#endif // !_INIPROCESSOR_H_
