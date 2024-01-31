#include "kdf11_aprocessor.h"
#include "../kd11_naconfig/kd11_naconfig.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

// To be able to pass the unique_ptr<KD11_AConfig> as a KD11Config pointer
// to KD11Processor, we have to pass the unique_ptr to KD11Processor via the
// init() function as KD11Processor's constructor is called before the
// unique_ptr is initialized.
KDF11_AProcessor::KDF11_AProcessor ()
	:
	KD11Processor (),
	kd11ConfigPtr {make_unique<KDF11_AConfig> ()}
{
	init (kd11ConfigPtr.get ());
}

void KDF11_AProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	// If a processFunction is found the key is found in the KDF11-A's
	// options, else it might be a KD11 common key.
	if (processFunction != nullptr)
		(this->*processFunction)(valueIterator->second);
	else
		KD11Processor::processValue (valueIterator);
}


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

unique_ptr<DeviceConfig> KDF11_AProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}