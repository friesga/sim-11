#include "configprocessor.h"

#include <iostream>

void ConfigProcessor::process (iniparser::File iniFile)
{
	iniparser::SectionVector topLevelSections = 
		iniFile.getTopLevelSections ();

	for (iniparser::Section* sectionPtr : topLevelSections)
		processSection (sectionPtr);
}

void ConfigProcessor::processSection (iniparser::Section* section)
{
	try
	{
		SectionProcessor *sectionProcessor = sectionProcessors.at (section->name());
		sectionProcessor->processSection (section);
	}
	catch (std::out_of_range)
	{
		throw std::invalid_argument {"Unknown section: " + section->name ()};
	}
}
