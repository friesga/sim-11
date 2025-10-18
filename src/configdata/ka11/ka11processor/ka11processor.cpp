#include "configdata/ka11/ka11processor/ka11processor.h"


void KA11Processor::processValue (iniparser::Section::ValueIterator valueIterator)
{}

void KA11Processor::checkConsistency ()
{}

void KA11Processor::processSubsection (iniparser::Section* subSection)
{}

DeviceConfig KA11Processor::getConfig ()
{
    return ka11Config_;
}