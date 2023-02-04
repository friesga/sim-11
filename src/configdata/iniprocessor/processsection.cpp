#include "iniprocessor.h"

#include <string>

using std::string;

void IniProcessor::processSection (iniparser::Section* section)
{
	// Get a pointer to the SectionProcessor factory to be used
	// to create the appropriate SectionProcessor to process this
	// section and create that SectionProcessor.
	SectionProcessorFactory factory = 
		sectionProcessorFactories[section->name()];

	// Check a SectionProcessor factory exists for this section
	if (factory)
	{
		// Create the appropriate sectionProcessor to process this section...
		unique_ptr<SectionProcessor> sectionProcessor = (this->*factory) ();

		// ...and let it process this section.
		sectionProcessor->processSection (section);

		// Add the device configuration to the system configuration
		systemConfig.push_back (sectionProcessor->getConfig ());
	}
	else
		throw string("Unsupported section: ") + section->name ();
}