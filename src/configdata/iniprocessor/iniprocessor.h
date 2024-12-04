#ifndef _INIPROCESSOR_H_
#define _INIPROCESSOR_H_

#include "iniparser.h"
#include "configdata/deviceconfig/deviceconfig.h"
#include "configdata/systemconfig/systemconfig.h"
#include "../sectionprocessor/deviceconfigprocessor.h"
#include "configdata/msv11config/msv11config.h"
#include "../ba11nprocessor/ba11nprocessor.h"
#include "configdata/rl/rl11processor/rl11processor.h"
#include "configdata/rl/rlv11processor/rlv11processor.h"
#include "configdata/rl/rlv12processor/rlv12processor.h"
#include "configdata/rxv21/rxv21processor/rxv21processor.h"
#include "../bdv11processor/bdv11processor.h"
#include "configdata/serialconfig/dlv11processor/dlv11processor.h"
#include "../msv11processor/msv11processor.h"
#include "../kd11_naprocessor/kd11_naprocessor.h"
#include "../kdf11/kdf11_a/kdf11_aprocessor/kdf11_aprocessor.h"
#include "../kdf11/kdf11_b/kdf11_bprocessor/kdf11_bprocessor.h"
#include "../kdf11/kdf11_u/kdf11_uprocessor/kdf11_uprocessor.h"
#include "../ms11pprocessor/ms11pprocessor.h"
#include "../ba11lprocessor/ba11lprocessor.h"

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
	SystemConfig getSystemConfig ();

private:
	// Define the system configuration as a vector of (shared) pointers
	// to device configurations.
	SystemConfig systemConfig {};

	typedef unique_ptr<DeviceConfigProcessor> (IniProcessor::*DeviceConfigProcessorFactory) ();

    map<string, DeviceConfigProcessorFactory> deviceConfigProcessorFactories =
	{
		{"BA11-N",  &IniProcessor::create<BA11_NProcessor>},
		{"BDV11",   &IniProcessor::create<BDV11Processor>},
		{"DLV11-J", &IniProcessor::create<DLV11Processor>},
		{"MSV11",   &IniProcessor::create<MSV11Processor>},
		{"RL11",    &IniProcessor::create<RL11Processor>},
		{"RLV11",   &IniProcessor::create<RLV11Processor>},
		{"RLV12",   &IniProcessor::create<RLV12Processor>},
		{"RXV21",   &IniProcessor::create<RXV21Processor>},
		{"KD11-NA", &IniProcessor::create<KD11_NAProcessor>},
		{"KDF11-A", &IniProcessor::create<KDF11_AProcessor>},
		{"KDF11-B", &IniProcessor::create<KDF11_BProcessor>},
		{"KDF11-U", &IniProcessor::create<KDF11_UProcessor>},
		{"MS11-P",  &IniProcessor::create<MS11PProcessor>},
		{"BA11-L",  &IniProcessor::create<BA11_LProcessor>}
	};

    void processSection (iniparser::Section* section);

	template <typename T>
	unique_ptr<DeviceConfigProcessor> create ();
};

template <typename T>
unique_ptr<DeviceConfigProcessor> IniProcessor::create ()
{
	return make_unique<T> ();
}

#endif // !_INIPROCESSOR_H_
