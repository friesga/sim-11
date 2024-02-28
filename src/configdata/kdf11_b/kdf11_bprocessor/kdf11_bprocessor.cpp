#include "kdf11_bprocessor.h"
#include "../sluprocessor/sluprocessor.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

// To be able to pass the unique_ptr<KD11_BConfig> as a KD11Config pointer
// to KD11Processor, we have to pass the unique_ptr to KD11Processor via the
// init() function as KD11Processor's constructor is called before the
// unique_ptr is initialized.
KDF11_BProcessor::KDF11_BProcessor ()
	:
	KD11Processor (),
	kd11ConfigPtr {make_unique<KDF11_BConfig> ()}
{
	init (kd11ConfigPtr.get ());
}

void KDF11_BProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	// If a processFunction is found the key is found in the KDF11-A's
	// options, else it might be a KD11 common key.
	if (processFunction != nullptr)
		(this->*processFunction)(valueIterator->second);
	else
		KD11Processor::processValue (valueIterator);
}

// A KDF11-B module comprises besides the processor two serial line units.
void KDF11_BProcessor::processSubsection (iniparser::Section *subSection)
{
	if (subSection->name().substr(0, 4) != "SLU")
		throw std::invalid_argument {"Unknown KDF11-B subsection: " + 
			subSection->name()};

	if (sluDefined)
		throw std::invalid_argument {"Double specification for KDF11-B " + 
			subSection->name()};
	sluDefined = true;

	SLUProcessor sluProcessor {};
	sluProcessor.processSection (subSection);

	// Add the unit configuration to the Rl device configuration
	kd11ConfigPtr->sluConfig = sluProcessor.getConfig ();
}

unique_ptr<DeviceConfig> KDF11_BProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}