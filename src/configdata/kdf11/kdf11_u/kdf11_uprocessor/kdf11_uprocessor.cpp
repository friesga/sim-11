#include "kdf11_uprocessor.h"
#include "configdata/kdf11/slu/sluprocessor/sluprocessor.h"
#include "touint.h"

#include <variant>

using std::move;
using std::invalid_argument;
using std::get;

void KDF11_UProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key in KDF11-U section");

	(this->*processFunction)(valueIterator->second);
}

void KDF11_UProcessor::checkConsistency ()
{
}

void KDF11_UProcessor::processSubsection (iniparser::Section* subSection)
{}

void KDF11_UProcessor::processPowerUpMode (iniparser::Value value)
{
	KD11Processor kd11Procesor;
	kdf11_uConfigPtr->powerUpMode =
		kd11Procesor.processPowerUpMode (value);

	// Valid power-up options for the KDF11-U are Vector and Bootstrap.
	if (kdf11_uConfigPtr->powerUpMode == KD11Config::PowerUpMode::ODT)
        throw invalid_argument {"Invalid KDF11-U power-up_mode, must be Vector or Bootstrap"};
}

void KDF11_UProcessor::processKernelHaltMode (iniparser::Value value)
{
	map<string, KDF11_UConfig::KernelHaltMode>::iterator iter;

	if ((iter = validKernelHaltModes.find (value.asString ())) !=
			validKernelHaltModes.end ())
		kdf11_uConfigPtr->kernelHaltMode = iter->second;
	else
		throw invalid_argument {"Incorrect KDF11-U kernel_halt value"};
}

// The boot address is either 0165000 or 173000.
void  KDF11_UProcessor::processBootAddress (iniparser::Value value)
{
	u16 bootAddress {0};
	try
	{
		bootAddress = touint<u16> (value.asString ());
	}
	catch (std::invalid_argument const&)
	{
		throw invalid_argument {"Incorrect boot address in KDF11-U section specified: " +
			value.asString ()};
	}

	// Check the starting address is on a 256-word boundary
	if (bootAddress != 0165000 && bootAddress != 0173000)
		throw invalid_argument {"KDF11-U boot address must be either 0165000 or 0173000"};

	kdf11_uConfigPtr->bootAddress = bootAddress;
}

DeviceConfig KDF11_UProcessor::getConfig ()
{
	return move (kdf11_uConfigPtr);
}