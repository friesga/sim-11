#include "kdf11_uprocessor.h"
#include "configdata/kdf11/slu/sluprocessor/sluprocessor.h"

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
{
	SectionProcessDef* sectionProcessDef;

	try
	{
		sectionProcessDef = &sectionProcess.at (subSection->name ());
	}
	catch (std::out_of_range const&)
	{
		throw std::invalid_argument {"Unknown KDF11-U subsection: " +
			subSection->name ()};
	}

	if (sectionProcessDef->defined)
		throw std::invalid_argument {"Double specification for KDF11-U " +
			subSection->name ()};
	sectionProcessDef->defined = true;

	(this->*sectionProcessDef->sectionProcessor) (subSection);
}

void KDF11_UProcessor::processPowerUpMode (iniparser::Value value)
{
	map<string, KDF11_UConfig::PowerUpMode>::iterator iter;

	if ((iter = validPowerUpModes.find (value.asString ())) !=
			validPowerUpModes.end ())
		kdf11_uConfigPtr->powerUpMode = iter->second;
	else
		throw invalid_argument {"Incorrect KDF11-U power-up_mode value"};
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

void KDF11_UProcessor::processSLUSubsection (iniparser::Section* subSection)
{
	SLUProcessor sluProcessor {};
	sluProcessor.processSection (subSection);

	// Add the configuration to the KDF11-U configuration
	kdf11_uConfigPtr->sluConfig = sluProcessor.getConfig ();
}

DeviceConfig KDF11_UProcessor::getConfig ()
{
	return move (kdf11_uConfigPtr);
}