#include "m792processor.h"

using std::invalid_argument;

void M792Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		// This exception will be catched and processed in 
		// SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in M792 section");

	(this->*processFunction)(valueIterator->second);
}

void M792Processor::processOption (iniparser::Value value)
{
	if (value.asString() != "M792-YB")
        throw std::invalid_argument ("Invalid option specified in M792 section");

	m792Config.option = M792Config::Option::M792_YB;
}


void M792Processor::checkConsistency ()
{
	if (!m792Config.option.hasValue ())
		throw std::invalid_argument {"No option specified in M792 section"};
}

void M792Processor::processSubsection (iniparser::Section* subSection)
{}

DeviceConfig M792Processor::getConfig ()
{
	return m792Config;
}
