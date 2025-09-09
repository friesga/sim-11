#include "ba11cprocessor.h"
#include "../ba11cconfig/ba11cconfig.h"
#include "../cabinetprocessor/cabinetprocessor.h"

using std::make_unique;
using std::move;
using std::invalid_argument;

BA11_CProcessor::BA11_CProcessor ()
{}

void BA11_CProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    Process processFunction = valueProcessors[valueIterator->first];

    if (processFunction == nullptr)
        // This exception will be catched and processed in 
        // SectionProcessor::processSection().
        throw std::out_of_range ("Unknown key in BA11-C section");

    (this->*processFunction)(valueIterator->second);
}

// Check the consistency of the configuration of the BA11-L. A valid cabinet
// position has to be specified.
void BA11_CProcessor::checkConsistency ()
{
    if (!ba11_cConfig.cabinetPosition.has_value ())
        throw invalid_argument {"Cabinet position not specified in BA11-C section"};

    if (!ba11_cConfig.operatorConsole.has_value ())
        throw invalid_argument {"No (valid) operator console specified in BA11-C section"};
}

void BA11_CProcessor::processSubsection (iniparser::Section* subSection)
{}

DeviceConfig BA11_CProcessor::getConfig ()
{
    return make_shared<BA11_CConfig> (ba11_cConfig);
}

void BA11_CProcessor::processCabinet (iniparser::Value value)
{
    ba11_cConfig.cabinetPosition =
        CabinetProcessor::processCabinetKey (value);
}

void BA11_CProcessor::processOperatorConsole (iniparser::Value value)
{
    map<string, BA11_CConfig::OperatorConsole>::iterator iter;

    if ((iter = availableConsoles.find (value.asString ())) !=
            availableConsoles.end ())
        ba11_cConfig.operatorConsole = iter->second;
    else
        throw invalid_argument {"Invalid BA11-C console specified"};
}