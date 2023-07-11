#ifndef _BDV11PROCESSOR_H_
#define _BDV11PROCESSOR_H_

#include "../sectionprocessor/sectionprocessor.h"
#include "../bdv11config/bdv11config.h"

#include <memory>
#include <map>
#include <string>

using std::unique_ptr;
using std::map;
using std::string;

class BDV11Processor : public SectionProcessor
{
	unique_ptr<BDV11Config> bdv11ConfigPtr {nullptr};

    // Define process as a pointer to a BDV11Processor member function
	// with a iniparser::Value argument and returning void.

	typedef void (BDV11Processor::*Process)(iniparser::Value);
	
	map<string, Process> valueProcessors =
	{
		{"cpu_tests", &BDV11Processor::processCpuTests},
		{"memory_tests", &BDV11Processor::processMemoryTests},
		{"decnet_boot", &BDV11Processor::processDecnetBoot},
		{"console_dialog", &BDV11Processor::processConsoleDialog},
		{"boot_device", &BDV11Processor::processBootDevice},
	};

    void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section *subSection);
	void processCpuTests (iniparser::Value value);
	void processMemoryTests (iniparser::Value value);
	void processDecnetBoot (iniparser::Value value);
	void processConsoleDialog (iniparser::Value value);
	void processBootDevice (iniparser::Value value);

public:
	BDV11Processor ();
	unique_ptr<DeviceConfig> getConfig ();
};


#endif // _BDV11PROCESSOR_H_