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
	if (value.asString() == "RK05")
		bdv11ConfigPtr->bootDevice = BDV11Config::BootDevice::RK05;
	else if (value.asString() == "RL01")
		bdv11ConfigPtr->bootDevice = BDV11Config::BootDevice::RL01;
	else if (value.asString() == "RX01")
		bdv11ConfigPtr->bootDevice = BDV11Config::BootDevice::RX01;
	else if (value.asString() == "RX02")
		bdv11ConfigPtr->bootDevice = BDV11Config::BootDevice::RX02;
	else if (value.asString() == "BDV11_ROM")
		bdv11ConfigPtr->bootDevice = BDV11Config::BootDevice::BDV11ROM;
	else 
		throw std::invalid_argument {"Incorrect BDV11 boot device: " + 
			value.asString()};
}

// Check the consistency of the configuration of the BDV11 controller. Currently
// this is a void.
void BDV11Processor::checkConsistency ()
{
}

void BDV11Processor::processSubsection (iniparser::Section *subSection)
{}

unique_ptr<DeviceConfig> BDV11Processor::getConfig ()
{
	return move (bdv11ConfigPtr);
}