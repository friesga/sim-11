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

unique_ptr<DeviceConfig> KDF11_AProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}