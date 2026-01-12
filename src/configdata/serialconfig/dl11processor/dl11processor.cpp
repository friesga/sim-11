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

	if (dl11Config.baseAddress < 0174000)
			throw invalid_argument {"DL11 base address must be in range 0174000 - 0177776"};

	if (dl11Config.baseAddress & 07)
		throw std::invalid_argument {"DL11 bus address must be on a eight byte boundary"};
}

void DL11Processor::processVector (iniparser::Value value)
{
	try
	{
		dl11Config.baseVector = touint<u16> (value.asString ());
	}
	catch (invalid_argument const&)
	{
		throw invalid_argument {"Incorrect vector address in DL11 section specified: " +
			value.asString ()};
	}

	if (dl11Config.baseVector > 0376)
        throw invalid_argument {"DL11 vector address must be in range 0000 - 0376"};
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
