#include "ba11nprocessor.h"
#include "../ba11nconfig/ba11nconfig.h"
#include "../cabinetprocessor/cabinetprocessor.h"


using std::make_unique;
using std::move;
using std::invalid_argument;

BA11_NProcessor::BA11_NProcessor ()
{
	ba11_nConfigPtr = make_unique<BA11_NConfig> ();
}

void BA11_NProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];

	if (processFunction == nullptr)
        // This exception will be catched and processed in 
        // SectionProcessor::processSection().
		throw std::out_of_range ("Unknown key in BA11-N section");

    (this->*processFunction)(valueIterator->second);
}

// Check the consistency of the configuration of the BA11-N. A valid cabinet
// position has to be specified.
void BA11_NProcessor::checkConsistency ()
{
    if (ba11_nConfigPtr->cabinetPosition == nullptr)
        throw invalid_argument {"Cabinet position not specified in BA11-N section"};
}

void BA11_NProcessor::processSubsection (iniparser::Section *subSection)
{}

DeviceConfig BA11_NProcessor::getConfig ()
{
	return move (ba11_nConfigPtr);
}

void BA11_NProcessor::processLogo (iniparser::Value value)
{
	map<string, BA11_NConfig::Logo>::iterator iter;

    if ((iter = availableLogos.find (value.asString ())) != 
            availableLogos.end ())
        ba11_nConfigPtr->logo = iter->second;
    else
        throw invalid_argument {"Unavailable BA11-N logo selected"};
}

void BA11_NProcessor::processCabinet (iniparser::Value value)
{
    ba11_nConfigPtr->cabinetPosition = 
        CabinetProcessor::processCabinetKey (value);
}