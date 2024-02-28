#include "kdf11_bprocessor.h"
#include "../sluprocessor/sluprocessor.h"
#include "configdata/bdv11processor/bdv11processor.h"

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
	if (subSection->name() == "SLU")
		processSLUSubsection (subSection);
	else if (subSection->name() == "BDV11")
		processBDV11Subsection (subSection);
	else
		throw std::invalid_argument {"Unknown KDF11-B subsection: " + 
			subSection->name()};
}

void KDF11_BProcessor::processSLUSubsection (iniparser::Section *subSection)
{
	if (sluDefined)
		throw std::invalid_argument {"Double specification for KDF11-B " + 
			subSection->name()};
	sluDefined = true;

	SLUProcessor sluProcessor {};
	sluProcessor.processSection (subSection);

	// Add the configuration to the KDF11-B configuration
	kd11ConfigPtr->sluConfig = sluProcessor.getConfig ();
}

void KDF11_BProcessor::processBDV11Subsection (iniparser::Section *subSection)
{
	if (bdv11Defined)
		throw std::invalid_argument {"Double specification for KDF11-B " + 
			subSection->name()};
	bdv11Defined = true;

	BDV11Processor bdv11Processor {};
	bdv11Processor.processSection (subSection);

	// Add the unit configuration to the Rl device configuration
	kd11ConfigPtr->bdv11Config = bdv11Processor.getConfig ();
}

unique_ptr<DeviceConfig> KDF11_BProcessor::getConfig ()
{
	return move (kd11ConfigPtr);
}