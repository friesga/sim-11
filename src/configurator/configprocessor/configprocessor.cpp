#include "configprocessor.h"

#include <iostream>

void ConfigProcessor::process (iniparser::File iniFile)
{
	iniparser::SectionVector topLevelSections = 
		iniFile.getTopLevelSections ();

	for (iniparser::Section* sectionPtr : topLevelSections)
		processSection (sectionPtr);
}

// std::unique_ptr<DeviceConfig> ConfigProcessor::getConfig ()
DeviceConfig *ConfigProcessor::getConfig ()
{
	deviceConfigPtr->rlConfig = rlProcessor.getConfig ();
	return deviceConfigPtr.get();
}

void ConfigProcessor::processSection (iniparser::Section* section)
{
	try
	{
		SectionProcessor *sectionProcessor = sectionProcessors.at (section->name());
		sectionProcessor->processSection (section);
	}
	catch (std::out_of_range const &)
	{
		throw std::invalid_argument {"Unknown section: " + section->name ()};
	}
}
