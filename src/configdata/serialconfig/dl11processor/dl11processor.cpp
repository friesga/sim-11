#include "dl11processor.h"
#include "touint.h"

using std::invalid_argument;

void DL11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		// This exception will be catched and processed in 
		// SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in DL11 section");

	(this->*processFunction)(valueIterator->second);
}

void DL11Processor::processAddress (iniparser::Value value)
{
	try
	{
		dl11Config.baseAddress = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw invalid_argument {"Incorrect address in DL11 section specified: " +
			value.asString ()};
	}
}

void DL11Processor::processVector (iniparser::Value value)
{
	try
	{
		dl11Config.baseVector = touint<u16> (value.asString ());
	}
	catch (invalid_argument const&)
	{
		throw invalid_argument {"Incorrect vector in DL11 section specified: " +
			value.asString ()};
	}
}

void DL11Processor::checkConsistency ()
{
}

void DL11Processor::processSubsection (iniparser::Section* subSection)
{
}

DeviceConfig DL11Processor::getConfig ()
{
	return dl11Config;
}
