#include "kdf11_bprocessor.h"
#include "../sluprocessor/sluprocessor.h"
#include "configdata/bdv11processor/bdv11processor.h"

#include <variant>

using std::move;
using std::invalid_argument;
using std::get;

void KDF11_BProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
	Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
		throw std::out_of_range ("Unknown key in KDF11-B section");

    (this->*processFunction)(valueIterator->second);
}

void KDF11_BProcessor::checkConsistency ()
{}

void KDF11_BProcessor::processSubsection (iniparser::Section *subSection)
{
	SectionProcessDef* sectionProcessDef;

	try
	{
		sectionProcessDef = &sectionProcess.at (subSection->name());
	}
	catch (std::out_of_range const&)
	{
		throw std::invalid_argument {"Unknown KDF11-B subsection: " + 
			subSection->name()};
	}

	if (sectionProcessDef->defined)
		throw std::invalid_argument {"Double specification for KDF11-B " + 
			subSection->name()};
	sectionProcessDef->defined = true;

	(this->*sectionProcessDef->sectionProcessor) (subSection);
}

void KDF11_BProcessor::processPowerUpMode (iniparser::Value value)
{
	KD11Processor kd11Procesor;
	kdf11_bConfigPtr->powerUpMode = 
		kd11Procesor.processPowerUpMode (value);
}

void KDF11_BProcessor::processSLUSubsection (iniparser::Section *subSection)
{
	SLUProcessor sluProcessor {};
	sluProcessor.processSection (subSection);

	// Add the configuration to the KDF11-B configuration
	kdf11_bConfigPtr->sluConfig = sluProcessor.getConfig ();
}

void KDF11_BProcessor::processBDV11Subsection (iniparser::Section *subSection)
{
	BDV11Processor bdv11Processor {};
	bdv11Processor.processSection (subSection);

	// Add the unit configuration to the Rl device configuration
	kdf11_bConfigPtr->bdv11Config = 
		get<shared_ptr<BDV11Config>> (bdv11Processor.getConfig ());
}

DeviceConfig KDF11_BProcessor::getConfig ()
{
	return move (kdf11_bConfigPtr);
}