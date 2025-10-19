#include "configdata/ka11/ky11_aprocessor/ky11_aprocessor.h"

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
{}

void KY11_AProcessor::checkConsistency ()
{}

void KY11_AProcessor::processSubsection (iniparser::Section* subSection)
{}

KY11_AConfig KY11_AProcessor::getConfig ()
{
    return ky11_aConfig_;
}