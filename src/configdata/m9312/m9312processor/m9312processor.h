#ifndef _M9312PROCESSOR_H
#define _M9312PROCESSOR_H

#include "configdata/sectionprocessor/deviceconfigprocessor.h"
#include "../m9312config/m9312config.h"

#include <memory>
#include <map>
#include <string>

using std::shared_ptr;
using std::unique_ptr;
using std::map;
using std::string;

class M9312Processor : public DeviceConfigProcessor
{
	unique_ptr<M9312Config> m9312ConfigPtr {nullptr};

	// Define Process as a pointer to a M9312Processor member function
	// with a iniparser::Value argument and returning void.
	typedef void (M9312Processor::* Process)(iniparser::Value);

	map<string, Process> valueProcessors =
	{
		{"diag-rom",  &M9312Processor::processDiagnosticROM},
		{"boot-roms", &M9312Processor::processBootROMs},
		{"starting-address", &M9312Processor::processStartingAddress},
		{"power-up-boot-enable", &M9312Processor::processPowerUpBootEnable}
	};

	map<string, M9312Config::DiagROMType> diagROMSpec =
	{
        {"23-248F1", M9312Config::DiagROMType::_23_248F1},
		{"23-446F1", M9312Config::DiagROMType::_23_446F1},
		{"23-616F1", M9312Config::DiagROMType::_23_616F1},
		{"23-774F1", M9312Config::DiagROMType::_23_774F1}
    };

	map<string, M9312Config::BootROMType> bootROMSpec =
	{
		{"23-751A9", M9312Config::BootROMType::_23_751A9},
		{"23-752A9", M9312Config::BootROMType::_23_752A9},
		{"23-753A9", M9312Config::BootROMType::_23_753A9},
		{"23-755A9", M9312Config::BootROMType::_23_755A9},
		{"23-756A9", M9312Config::BootROMType::_23_756A9},
		{"23-757A9", M9312Config::BootROMType::_23_757A9},
		{"23-758A9", M9312Config::BootROMType::_23_758A9},
		{"23-759A9", M9312Config::BootROMType::_23_759A9},
		{"23-760A9", M9312Config::BootROMType::_23_760A9},
		{"23-761A9", M9312Config::BootROMType::_23_761A9},
		{"23-764A9", M9312Config::BootROMType::_23_764A9},
		{"23-765B9", M9312Config::BootROMType::_23_765B9},
		{"23-767A9", M9312Config::BootROMType::_23_767A9},
		{"23-811A9", M9312Config::BootROMType::_23_811A9},
		{"23-862A9", M9312Config::BootROMType::_23_862A9},
		{"23-863A9", M9312Config::BootROMType::_23_863A9},
		{"23-864A9", M9312Config::BootROMType::_23_864A9},
		{"23-E32A9", M9312Config::BootROMType::_23_E32A9},
		{"23-E33A9", M9312Config::BootROMType::_23_E33A9},
		{"23-E39A9", M9312Config::BootROMType::_23_E39A9}
	};

	void processValue (iniparser::Section::ValueIterator valueIterator);
	void checkConsistency ();
	void processSubsection (iniparser::Section* subSection);
	void processDiagnosticROM (iniparser::Value value);
	void processBootROMs (iniparser::Value value);
    void processStartingAddress (iniparser::Value value);
	void processPowerUpBootEnable (iniparser::Value value);
	bool addressInRange (u16 address);

public:
	M9312Processor ();
	DeviceConfig getConfig () override;
};


#endif // _M9312PROCESSOR_H_