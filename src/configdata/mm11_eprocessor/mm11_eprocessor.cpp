#include "mm11_eprocessor.h"
#include "touint.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;

void MM11EProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key");

	(this->*processFunction)(valueIterator->second);
}

void MM11EProcessor::processStartingAddress (iniparser::Value value)
{
	try
	{
		mm11eConfig.startingAddress = touint<u32> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw invalid_argument {"Incorrect starting address in MM11-E section specified: " +
			value.asString ()};
	}
}

void MM11EProcessor::processMemoryExtension (iniparser::Value value)
{
	try
	{
		mm11eConfig.memorySizeKB = value.asBool () ? 16384 : 8192;
	}
	catch (invalid_argument const&)
	{
		throw invalid_argument {"Value of memory_extension must be \'true\' or \'false\'"};
	}
}

// Check the consistency of the configuration of the MSV11 memory.
void MM11EProcessor::checkConsistency ()
{
	if (mm11eConfig.startingAddress >= 01000000)
		throw invalid_argument {"MM11-E maximum starting address is 0777776"};

	if (mm11eConfig.startingAddress % 020000 != 0)
		throw invalid_argument {"MM11-E address must start at 4KW boundary"};
}

void MM11EProcessor::processSubsection (iniparser::Section* subSection)
{
}

DeviceConfig MM11EProcessor::getConfig ()
{
	return mm11eConfig;
}
