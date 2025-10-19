#include "configdata/ka11/ka11processor/ka11processor.h"
#include "configdata/ka11/ky11_aprocessor/ky11_aprocessor.h"

void KA11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

void KA11Processor::checkConsistency ()
{}

void KA11Processor::processSubsection (iniparser::Section* subSection)
{
	KY11_AProcessor ky11_aprocessor {subSection};
	ky11_aprocessor.processSection (subSection);

	ka11Config_.ky11_aConfig_ = ky11_aprocessor.getConfig ();
}

DeviceConfig KA11Processor::getConfig ()
{
    return ka11Config_;
}