#include "configprocessor.h"

#include <iostream>

// This function is the entry point for the processing of the configuration
// file. It gets a vector with the data on the top level sections in
// the configuration file and then processes these sections.
void ConfigData::process (iniparser::File iniFile)
{
	iniparser::SectionVector topLevelSections = 
		iniFile.getTopLevelSections ();

	for (iniparser::Section* sectionPtr : topLevelSections)
		processSection (sectionPtr);
}

// This function returns the processed configuration data.
// std::unique_ptr<DeviceConfig> ConfigData::getConfig ()
DeviceConfig *ConfigData::getConfig ()
{
	deviceConfigPtr->rlConfig = rlProcessor.getConfig ();
	deviceConfigPtr->rxConfig = rxProcessor.getConfig ();
	return deviceConfigPtr.get();
}

// This function processes one top level section in the configuration file.
void ConfigData::processSection (iniparser::Section* section)
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
