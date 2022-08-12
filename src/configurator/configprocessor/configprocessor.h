#ifndef _CONFIGPROCESSOR_H_
#define _CONFIGPROCESSOR_H_

#include "../rlprocessor/rlprocessor.h"

#include <memory>

struct DeviceConfig
{
	RlConfig *rlConfig = nullptr;
};

class ConfigProcessor
{
	std::unique_ptr<DeviceConfig> deviceConfigPtr = 
		std::make_unique<DeviceConfig> ();
	RlProcessor rlProcessor;

	std::map<std::string, SectionProcessor*> sectionProcessors =
	{
		{"RL", &rlProcessor}
		
	};

	void processSection (iniparser::Section* section);

public:
	void process (iniparser::File iniFile);
	DeviceConfig *getConfig ();
};

#endif // _CONFIGPROCESSOR_H_