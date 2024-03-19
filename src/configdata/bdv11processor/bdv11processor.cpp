#include "bdv11processor.h"

#include <utility>

using std::make_unique;
using std::move;
using std::invalid_argument;
using std::out_of_range;

BDV11Processor::BDV11Processor ()
{
	bdv11ConfigPtr = make_unique<BDV11Config> ();
}

void BDV11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];
    (this->*processFunction)(valueIterator->second);
}

void BDV11Processor::processBootROM (iniparser::Value value)
{
	try
	{
		bdv11ConfigPtr->bootROM = bootROMSpec.at (value.asString ());
	}
	catch (out_of_range const &)
	{
		throw invalid_argument {"Incorrect BDV11 boot rom: " + 
			value.asString()};
	}
}

void BDV11Processor::processCpuTests (iniparser::Value value)
{
	bdv11ConfigPtr->cpuTests = value.asBool();
}

void BDV11Processor::processMemoryTests (iniparser::Value value)
{
	bdv11ConfigPtr->memoryTests = value.asBool();
}

void BDV11Processor::processDecnetBoot (iniparser::Value value)
{
	bdv11ConfigPtr->decnetBoot = value.asBool();
}

void BDV11Processor::processConsoleDialog (iniparser::Value value)
{
	bdv11ConfigPtr->consoleDialog = value.asBool();
}

void BDV11Processor::processBootDevice (iniparser::Value value)
{
	try
	{
		bdv11ConfigPtr->bootDevice = bootDeviceSpec.at (value.asString ());
	}
	catch (out_of_range const &)
	{
		throw invalid_argument  {"Incorrect BDV11 boot device: " + 
			value.asString()};
	}
}

// Check the consistency of the configuration of the BDV11 controller. Currently
// this is a void.
void BDV11Processor::checkConsistency ()
{
	if (bdv11ConfigPtr->bootDevice == BDV11Config::BootDevice::TU58 &&
		bdv11ConfigPtr->bootROM == BDV11Config::BootROM::BDV11)
		throw invalid_argument  {"TU58 as boot device is not supported in BDV11 ROMs"};
}

void BDV11Processor::processSubsection (iniparser::Section *subSection)
{}

DeviceConfigVariant BDV11Processor::getConfig ()
{
	return move (bdv11ConfigPtr);
}