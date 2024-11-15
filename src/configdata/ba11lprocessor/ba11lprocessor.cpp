#include "ba11lprocessor.h"
#include "../ba11lconfig/ba11lconfig.h"
#include "../cabinetprocessor/cabinetprocessor.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

BA11_LProcessor::BA11_LProcessor ()
{
    ba11_lConfigPtr = make_unique<BA11_LConfig> ();
}

void BA11_LProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];

    if (processFunction == nullptr)
        // This exception will be catched and processed in 
        // SectionProcessor::processSection().
        throw std::out_of_range ("Unknown key in BA11-L section");

    (this->*processFunction)(valueIterator->second);
}

// Check the consistency of the configuration of the BA11-L. A valid cabinet
// position has to be specified.
void BA11_LProcessor::checkConsistency ()
{
    if (ba11_lConfigPtr->cabinetPosition == nullptr)
        throw invalid_argument {"Cabinet position not specified in BA11-L section"};
}

void BA11_LProcessor::processSubsection (iniparser::Section* subSection)
{}

DeviceConfig BA11_LProcessor::getConfig ()
{
    return move (ba11_lConfigPtr);
}

void BA11_LProcessor::processCabinet (iniparser::Value value)
{
    ba11_lConfigPtr->cabinetPosition =
        CabinetProcessor::processCabinetKey (value);
}