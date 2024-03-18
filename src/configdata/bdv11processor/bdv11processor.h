#ifndef _BDV11PROCESSOR_H_
#define _BDV11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../bdv11config/bdv11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::shared_ptr;
using std::map;
using std::string;

class BDV11Processor : public SectionProcessor
{
public:
	BDV11Processor ();
	shared_ptr<DeviceConfig> getConfig ();

private:
	unique_ptr<BDV11Config> bdv11ConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.

	typedef void (BDV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"boot_rom", &BDV11Processor::processBootROM},
		{"cpu_tests", &BDV11Processor::processCpuTests},
		{"memory_tests", &BDV11Processor::processMemoryTests},
		{"decnet_boot", &BDV11Processor::processDecnetBoot},
		{"console_dialog", &BDV11Processor::processConsoleDialog},
		{"boot_device", &BDV11Processor::processBootDevice},
	};

	map<string, BDV11Config::BootROM> bootROMSpec =
	{
		{"bdv11",   BDV11Config::BootROM::BDV11},
		{"kdf11-b", BDV11Config::BootROM::KDF11_BA}
	};

	map<string, BDV11Config::BootDevice> bootDeviceSpec =
	{
		{"RK05",	  BDV11Config::BootDevice::RK05},
		{"RL01",	  BDV11Config::BootDevice::RL01},
		{"TU58",	  BDV11Config::BootDevice::TU58},
		{"RX01",	  BDV11Config::BootDevice::RX01},
		{"RX02",	  BDV11Config::BootDevice::RX02},
		{"BDV11_ROM", BDV11Config::BootDevice::BDV11ROM}
	};


    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processBootROM (iniparser::Value value);
	void processCpuTests (iniparser::Value value);
	void processMemoryTests (iniparser::Value value);
	void processDecnetBoot (iniparser::Value value);
	void processConsoleDialog (iniparser::Value value);
	void processBootDevice (iniparser::Value value);
};


#endif // _BDV11PROCESSOR_H_