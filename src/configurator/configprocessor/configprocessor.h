#ifndef _CONFIGPROCESSOR_H_
#define _CONFIGPROCESSOR_H_

#include "../rlprocessor/rlprocessor.h"

class ConfigProcessor
{
	RlProcessor rlProcessor;

	std::map<std::string, SectionProcessor*> sectionProcessors =
	{
		{"RL", &rlProcessor}
		
	};

	void processSection (iniparser::Section* section);

public:
	void process (iniparser::File iniFile);
};

#endif // _CONFIGPROCESSOR_H_