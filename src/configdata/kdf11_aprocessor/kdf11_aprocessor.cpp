#include "kdf11_aprocessor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

using std::move;
using std::invalid_argument;

void KDF11_AProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key in KDF11-A section");

    (this->*processFunction)(valueIterator->second);
}

void KDF11_AProcessor::checkConsistency ()
{}

void KDF11_AProcessor::processSubsection (iniparser::Section *subSection)
{}

void KDF11_AProcessor::processKTF11_A (iniparser::Value value)
{
	try
	{
		kd11ConfigPtr->ktf11_a_present = value.asBool ();
	}
	catch (invalid_argument const &)
	{
		throw invalid_argument {"Value of KTF11_A must be \'true\' or \'false\'"};
	}
}

// The high byte (bits <15:09>) of the starting address are selectable.
// The starting address can reside on any 256-word boundary in the lower
// 32K of memory address space (EK-KDF11-UG-PR2).
void KDF11_AProcessor::processStartingAddress (iniparser::Value value)
{
	u16 startingAddress {0};
	try
	{
		startingAddress = touint<u16> (value.asString());
	}
	catch (std::invalid_argument const &)
	{
		throw invalid_argument {"Incorrect starting address in KDF11-A section specified: " + 
			value.asString()};
	}

	// Check the starting address is on a 256-word boundary
	if ((startingAddress % 01000) != 0)
		throw invalid_argument {"KDF11-A starting address must reside on 256-word boundary"};

	kd11ConfigPtr->startingAddress = startingAddress;
}

void KDF11_AProcessor::processPowerUpMode (iniparser::Value value)
{
	KD11Processor kd11Procesor;
	kd11ConfigPtr->powerUpMode = 
		kd11Procesor.processPowerUpMode (value);
}

DeviceConfigVariant KDF11_AProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}