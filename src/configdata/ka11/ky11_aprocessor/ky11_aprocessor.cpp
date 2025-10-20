#include "configdata/ka11/ky11_aprocessor/ky11_aprocessor.h"
#include "configdata/cabinetprocessor/cabinetprocessor.h"

// A KY11_AProcessor will be created for evenry subsection of a KA11-A
// section. The only subsection allowed is the KY11-A subsection, so check
// that here.
KY11_AProcessor::KY11_AProcessor (iniparser::Section* subSection)
{
    checkType (subSection->name ());
}

void KY11_AProcessor::checkType (string type)
{
    if (type != "KY11-A")
        throw std::invalid_argument ("Unexpected KA11-A subsection");
}

void KY11_AProcessor::processValue (iniparser::Section::ValueIterator valueIterator)
{
    // Throw exception for non-existing key?
    Process processFunction = valueProcessors[valueIterator->first];

    if (processFunction == nullptr)
        // This exception will be catched and processed in 
        // SectionProcessor::processSection().
        throw std::out_of_range ("Unknown key in KY11-A section");

    (this->*processFunction)(valueIterator->second);
}

void KY11_AProcessor::processCabinet (iniparser::Value value)
{
    try
    {
        ky11_aConfig_.cabinetPosition =
            CabinetProcessor::processCabinetKey (value);
    }
    catch (std::invalid_argument const& except)
    {
        throw std::invalid_argument ("Invalid cabinet position in KY11-A section");
    }
}

void KY11_AProcessor::checkConsistency ()
{
    if (ky11_aConfig_.cabinetPosition == nullopt)
        throw std::invalid_argument ("KY11-A cabinet position not specified");
}

void KY11_AProcessor::processSubsection (iniparser::Section* subSection)
{}

KY11_AConfig KY11_AProcessor::getConfig ()
{
    return ky11_aConfig_;
}